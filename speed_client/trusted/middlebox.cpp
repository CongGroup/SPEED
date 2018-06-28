#include "middlebox.h"

#include "pktreader.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include "stdlib.h"
#include <cstring>
#include <vector>
#include <pcre.h>
#include <algorithm>

#include "FunctionDB.h"

using std::vector;

extern bool dedup_switch;

struct tuple4
{
	tuple4()
	{
		src_ip = dst_ip = src_port = dst_port = size = 0;
		memset(data, 0, sizeof(data));
		memset(pcre_output, 0, sizeof(pcre_output));
	};

	bool operator==(const tuple4& right)
	{
		return src_ip == right.src_ip&&
			dst_ip == right.dst_ip&&
			src_port == right.src_port&&
			dst_port == right.dst_port;
	}

	bool appendData(const uint8_t* new_data, const int data_size)
	{
		if ((size + data_size) < tcp_reassembly_buffer_size)
		{
			memcpy(data + size, new_data, data_size);
			size += data_size;

			return true;
		}
		else
		return false;
	}

	void doPcre(char** patterns)
	{
		pcreTimes++;
		if (dedup_switch)
		{
			doPcre_with_dedup(patterns);
		}
		else
		{
			doPcre_without_dedup(patterns);
		}
	}

	void doPcre_without_dedup(char** patterns)
	{
		char** patternPos = patterns;
		while (*patternPos != 0)
		{
			char*pattern = *(patternPos++);
			if (!size)
				return;
			const char* m_error;
			int m_erroffset;
			pcre* m_pcre;
			m_pcre = pcre_compile(pattern, 0, &m_error, &m_erroffset, NULL);
			int rc = pcre_exec(m_pcre, 0, data, size, 0, 0, pcre_output, pcre_output_size);
			pcre_free(m_pcre);

			if (rc > 0)
				pcreResValue += rc;
		}

		//clear tuple4
		size = 0;
		memset(data, 0, sizeof(data));
		memset(pcre_output, 0, sizeof(pcre_output));
	}

	void doPcre_with_dedup(char** patterns)
	{
		DEDUP_FUNCTION_INIT;
		byte* pb;
		int rc = pcreResValue;

		int patternLen = 0;
		char** patternPos = patterns;
		while (*patternPos != 0)
		{
			patternLen += strlen(*(patternPos++));
		}

		input_with_r_buffer_size = patternLen + sizeof(size) + sizeof(data) + HASH_SIZE;
		input_with_r_buffer = new byte[input_with_r_buffer_size];
		memset(input_with_r_buffer, 0, input_with_r_buffer_size);

		output_buffer_size = sizeof(int);
		output_buffer = new byte[output_buffer_size];
		memset(output_buffer, 0, output_buffer_size);

		pb = input_with_r_buffer;
		COPY_OBJECT(pb, size);
		COPY_OBJECT(pb, data);
		patternPos = patterns;
		while (*patternPos != 0)
		{
			COPY_POINTER(pb, *patternPos, strlen(*patternPos));
			patternPos++;
		}

		DEDUP_FUNCTION_QUERY;
		doPcre_without_dedup(patterns);
		output_true_size = output_buffer_size;

		pb = output_buffer;
		rc = pcreResValue - rc;
		COPY_OBJECT(pb, rc);


		DEDUP_FUNCTION_UPDATE;
		if (doDedup)
		{
			pb = output_buffer;
			READ_OBJECT(pb, rc);
			pcreResValue += rc;

			size = 0;
			memset(data, 0, sizeof(data));
			memset(pcre_output, 0, sizeof(pcre_output));
		}
		// clear memory
		delete[] input_with_r_buffer;
		delete[] output_buffer;
	}

	uint32_t src_ip;
	uint32_t dst_ip;
	uint16_t src_port;
	uint16_t dst_port;

	char data[tcp_reassembly_buffer_size];
	int pcre_output[pcre_output_size * 3];
	int size;

	static int pcreResValue;
	static int pcreTimes;
};

int tuple4::pcreResValue = 0;
int tuple4::pcreTimes = 0;


int middlebox_run(const char * pcap_path, const int pkt_used)
{
	char(*packets)[pkt_buffer_size] = 0;
	int pkt_count = pkt_used;

	ocall_load_pkt_file(pcap_path, (char**)&packets, &pkt_count);
	eprintf("read %d packets.\n", pkt_count);

	vector<tuple4> streamList;
	streamList.reserve(max_stream_size);

	const int patternCount = 1;

	char** patterns = new char*[patternCount +1];
	char* simplePattern = "void";
	char* complexPattern = "^\s*struct\s+\w+\s+\**\s*\w+\s*=\s*\w+\((\w+(,)*)+\);";
	char* pattern = ".*";

	for (int i = 0; i < patternCount; i++)
	{
		patterns[i] = pattern;
	}
	patterns[patternCount] = 0;

	PktReader reader;

	for (int i = 0; i < pkt_count; i++, packets++)
	{
		reader.attach((const char*)packets);
		if (reader.getPayloadLength() >= pkt_buffer_size - 54)
		{
			eprintf("error pay load size %d: %d\n", i, reader.getPayloadLength());
			continue;
		}
		tuple4 t4;

		t4.src_ip = reader.getSrcIP();
		t4.dst_ip = reader.getDstIP();
		t4.src_port = reader.getSrcPort();
		t4.dst_port = reader.getDstPort();

		auto res = std::find(streamList.begin(), streamList.end(), t4);

		if (reader.getPayloadLength() > 0)
			if (res != streamList.end())
			{
				// find stream add it
				if (!res->appendData(reader.getPayload(), reader.getPayloadLength()))
				{
					//eprintf("pm when buffer full.\n");
					res->doPcre(patterns);
				}
				res->appendData(reader.getPayload(), reader.getPayloadLength());
			}
			else
			{
				// add new stream
				if (streamList.size() >= max_stream_size)
				{
					res = streamList.begin();
					//eprintf("pm when stream full.\n");
					res->doPcre(patterns);
					streamList.erase(res);
				}
				t4.appendData(reader.getPayload(), reader.getPayloadLength());
				streamList.push_back(std::move(t4));
				res = streamList.end() - 1;
			}


		if (reader.getPSHFlag() && res != streamList.end())
		{
			//eprintf("pm when push.\n");
			res->doPcre(patterns);
		}
		if (reader.getFINFlag() && res != streamList.end())
		{
			//eprintf("pm when fin.\n");
			res->doPcre(patterns);
			streamList.erase(res);
		}
	}
	
	eprintf("do pattern match times is %d.\n", tuple4::pcreTimes);
	eprintf("do pattern match res is %d.\n", tuple4::pcreResValue);

	return tuple4::pcreResValue;
}

void clearCounter()
{
	tuple4::pcreTimes = 0;
	tuple4::pcreResValue = 0;
}

