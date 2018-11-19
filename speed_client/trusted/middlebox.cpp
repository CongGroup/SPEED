#include "middlebox.h"

#include "pktreader.h"
#include "Enclave_t.h"
#include "sysutils.h"
#include "stdlib.h"
#include <cstring>
#include <string>
#include <vector>
#include <pcre.h>
#include <algorithm>
#include "dedupTool.h"
#include "pattern_loader.h"
#include "FunctionDB.h"
#include "Deduplicable.h"

using std::vector;
using std::string;

extern bool dedup_switch;

char** globe_patterns;

static int pcre_pattern_matching_wrapper(const string& text)
{
	static bool patternInit;
	static pcre** pcres;
	static int pcre_output[pcre_output_size * 3];

	if (!patternInit)
	{
		pcres = new pcre*[pattern_count];
		memset(pcres, 0, sizeof(pcre*)*pattern_count);

		const char* m_error;
		int m_erroffset;

		for (int i = 0; i < pattern_count; i++)
		{
			char*pattern = globe_patterns[i];
			if (pattern)
			{
				pcres[i] = pcre_compile(pattern, 0, &m_error, &m_erroffset, NULL);
			}
		}
		patternInit = true;
	}

	int res = 0;
	for (int i = 0; i < pattern_count; i++)
	{
		memset(pcre_output, 0, sizeof(pcre_output));
		int nn = pcre_exec(pcres[i], 0, text.data(), text.size(), 0, 0, pcre_output, pcre_output_size);
		if (nn > 0)
		{
			res += nn;
		}
	}
	return res;
}


struct tuple4
{
	tuple4()
	{
		src_ip = dst_ip = src_port = dst_port = size = 0;
		memset(data, 0, sizeof(data));
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

	void doPcre()
	{
		pcreTimes++;
		string text(data, size);
		if (dedup_switch)
		{
			Deduplicable<int, const string&> dedup_func("libpcre", "10.23", "pcre_pattern_matching_wrapper", pcre_pattern_matching_wrapper);
			pcreResValue += dedup_func(text);
		}
		else
		{
			pcreResValue += pcre_pattern_matching_wrapper(text);
		}

		//clear tuple4
		size = 0;
		memset(data, 0, sizeof(data));
	}

	uint32_t src_ip;
	uint32_t dst_ip;
	uint16_t src_port;
	uint16_t dst_port;

	char data[tcp_reassembly_buffer_size];
	int size;

	static unsigned long long pcreResValue;
	static unsigned long long pcreTimes;
};

unsigned long long tuple4::pcreResValue = 0;
unsigned long long tuple4::pcreTimes = 0;

char* strdup(const char* str)
{
	int len = strlen(str);
	char* res = (char*)malloc(len + 1);
	memcpy(res, str, len);
	res[len] = 0;
	return res;
}

int middlebox_run(const char * pcap_path, const int pkt_used)
{
	char(*packets)[pkt_buffer_size] = 0;

	int pkt_count = pkt_used;

	ocall_load_pkt_file(pcap_path, (char**)&packets, &pkt_count);
	eprintf("read %d packets.\n", pkt_count);

	vector<tuple4> streamList;
	streamList.reserve(max_stream_size);

	PatternSet snortSet;
	PatternLoader::load_pattern_array(snortSet);

	const int patternCount = snortSet.size();
	// need a extern patterns to indicate patterns end
	char** patterns = new char*[patternCount + 1];
	//char* simplePattern = "void";
	//char* complexPattern = "^\s*struct\s+\w+\s+\**\s*\w+\s*=\s*\w+\((\w+(,)*)+\);";
	//char* pattern = ".*";

	for (int i = 0; i < patternCount; i++)
	{
		patterns[i] = strdup(PatternLoader::binary_to_string(snortSet[i]).c_str());
	}
	patterns[patternCount] = 0;

	globe_patterns = patterns;

	eprintf("Use pattern count %d.\n", patternCount);

	PktReader reader;
	tuple4 t4;

	for (int i = 0; i < pkt_count; i++, packets++)
	{
		reader.attach((const char*)packets);
		int pktLen = reader.getPayloadLength();

		t4.src_ip = reader.getSrcIP();
		t4.dst_ip = reader.getDstIP();
		t4.src_port = reader.getSrcPort();
		t4.dst_port = reader.getDstPort();

		auto res = std::find(streamList.begin(), streamList.end(), t4);
		//eprintf("payloadLength is %d .\n", pktLen);

		if (res != streamList.end())
		{
			// find stream add it
			if (!res->appendData(reader.getPayload(), pktLen))
			{
				//eprintf("pm when buffer full.\n");
				res->doPcre();
			}
			res->appendData(reader.getPayload(), pktLen);
		}
		else
		{
			// add new stream
			if (streamList.size() >= max_stream_size)
			{
				res = streamList.begin();
				//eprintf("pm when stream full.\n");
				res->doPcre();
				streamList.erase(res);
			}

			streamList.push_back(t4);
			res = streamList.end() - 1;

			res->appendData(reader.getPayload(), pktLen);
		}

		//if (reader.getPSHFlag() && res != streamList.end())
		//{
		//	//eprintf("pm when push.\n");
		//	res->doPcre();
		//}
		if (reader.getFINFlag() && res != streamList.end())
		{
			//eprintf("pm when fin.\n");
			res->doPcre();
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

