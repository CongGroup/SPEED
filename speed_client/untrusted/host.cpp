#include "enclave_management.h"
#include "server.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include <string>
#include <fstream>
#include <set>
#include <pcap/pcap.h>
#include <netinet/in.h> 
#include <netinet/ip.h> 
#include <netinet/udp.h> 
#include <netinet/ether.h> 
#include <arpa/inet.h> 
#include "time.h"
#include <fcntl.h>
#include "../trusted/pktreader.h"

using namespace std;
using namespace CryptoPP;
// type 1 str data
// type 2 pkt data
void removeSameData(int type, string inputPath, string outputPath)
{
	set<string> bucket;
	SHA256 hash = SHA256();
	byte hashBuffer[SHA256::DIGESTSIZE]; 
	string strHash;

	if (type == 1)
	{
		ifstream fin(inputPath, ios::in);
		ofstream fout(outputPath, ios::out | ios::trunc);

		// read source file and output
		string oneLineStr;
		int funcPos, linePos;


		while (fin.good())
		{
			getline(fin, oneLineStr);

			auto sep = oneLineStr.find('\t');
			if (sep == string::npos)
				continue;
			oneLineStr = oneLineStr.substr(sep);

			hash.CalculateDigest(hashBuffer, (byte*)oneLineStr.data(), oneLineStr.size());
			strHash.assign((char*)hashBuffer, SHA256::DIGESTSIZE);

			if (bucket.insert(strHash).second)
				fout << oneLineStr << endl;
		}

		fin.close();
		fout.close();

	}
	else if (type == 2)
	{
		pcap_t *handle;			/* Session handle */
		pcap_dumper_t *handle_out = NULL;
		char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
		struct bpf_program fp;		/* The compiled filter */
		char filter_exp[] = "tcp";	/* The filter expression */
		bpf_u_int32 mask;		/* Our netmask */
		bpf_u_int32 net;		/* Our IP */
		PktReader reader;

		/* Open the session in promiscuous mode */
		handle = pcap_open_offline(inputPath.c_str(), errbuf);
		if (handle == NULL) {
			fprintf(stderr, "Couldn't open file %s: %s\n", inputPath.c_str(), errbuf);
		}

		/* Compile and apply the filter */
		if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
			fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		}
		if (pcap_setfilter(handle, &fp) == -1) {
			fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		}

		handle_out = pcap_dump_open(handle, outputPath.c_str());

		size_t maxPktCount = 10000;
		int loadPktCount = 0;
		char* curPktBuffer[2000];

		const uint8_t *packet;		/* The actual packet */
		struct pcap_pkthdr header;	/* The header that pcap gives us */
		int pageloadSize;

		while (maxPktCount > 0)
		{
			/* Grab a packet */
			packet = pcap_next(handle, &header);

			// eof
			if (!packet)
			{
				break;
			}

			// not full packet
			if (header.len > header.caplen)
			{
				continue;
			}

			// packet too big
			if (header.caplen >= pkt_buffer_size)
			{
				continue;
			}

			// tcp pageload size too big or zero
			reader.attach((const char*)packet);
			pageloadSize = reader.getPayloadLength();
			if (pageloadSize == 0 || pageloadSize > pkt_buffer_size - 54)
			{
				continue;
			}

			if (pageloadSize !=1380)
			{
				continue;
			}

			hash.CalculateDigest(hashBuffer, packet, header.caplen);
			strHash.assign((char*)hashBuffer, SHA256::DIGESTSIZE);

			if (bucket.insert(strHash).second)
				pcap_dump((u_char*)handle_out, &header, packet);
			else
				continue;

			++loadPktCount;
			--maxPktCount;

			if (loadPktCount % 1000 == 0)
			{
				printf("have convert %d packet\n", loadPktCount);
			}
		}
		/* And close the session */
		pcap_dump_close(handle_out);
		pcap_close(handle);
	}
}
static uint8_t* dataMac(const char* strMac, const int strLeng)
{
	static uint8_t dstMac[12];
	for (int i = 0, j = 0; i < strLeng; i++)
	{
		char c = strMac[i];
		if (c >= 48 && c <= 57)
		{
			dstMac[j++] = c - 48;
		}
		else if (c >= 65 && c <= 70)
		{
			dstMac[j++] = c - 55;
		}
		else if (c >= 97 && c <= 102)
		{
			dstMac[j++] = c - 87;
		}
		else
		{
			continue;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		dstMac[i] = (dstMac[i * 2] << 4) | (dstMac[i * 2 + 1]);
	}

	return dstMac;
}

// gen some pkts for tests
void pktGenerator(string outputPath, int pktCount, int flowSize, int pageloadSize)
{
	set<string> bucket;
	SHA256 hash = SHA256();
	byte hashBuffer[SHA256::DIGESTSIZE];
	string strHash;

	string inputPath = "./data/patternMatching/m57.pcap";

	pcap_t *handle;			/* Session handle */
	pcap_dumper_t *handle_out = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */

	/* Open the session in promiscuous mode */
	handle = pcap_open_offline(inputPath.c_str(), errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open file %s: %s\n", inputPath.c_str(), errbuf);
	}
	handle_out = pcap_dump_open(handle, outputPath.c_str());

	int randFile = open("/dev/urandom", O_RDONLY);
	if (-1 == randFile)
	{
		printf("open /dev/urandom error\n");
		return;
	}

	char* srcMac = "12:34:56:78:9a:bc";
	char* dstMac = "12:34:56:78:9a:bd";
	char* srcIP = "192.168.1.107";
	char* dstIP = "192.168.1.5";
	unsigned short srcPort = 10000;
	unsigned short dstPort = 10000;

	uint8_t packet_buffer[1600];		/* The actual packet */
	int pktSize = ether_len + ip_default_len + udp_default_len + pageloadSize;

	if (pktSize > 1514)
	{
		printf("error pktSize %d.\n", pktSize);
		return;
	}

	ether_header* ethH = (ether_header*)packet_buffer;
	memset(ethH, 0, sizeof(ethH));
	memcpy(ethH->ether_shost, dataMac(srcMac, strlen(srcMac)), 6);
	memcpy(ethH->ether_dhost, dataMac(dstMac, strlen(dstMac)), 6);
	ethH->ether_type = htons(ETHERTYPE_IP);

	iphdr* ipH = (iphdr*)(packet_buffer+ ether_len);
	memset(ipH, 0, sizeof(ipH));
	ipH->version = 4;
	uint8_t ipHeaderLen = ip_default_len;
	ipH->ihl = ipHeaderLen / 4;
	ipH->tos = IPTOS_ECN_NOT_ECT;
	ipH->tot_len = htons(pktSize-ether_len);
	ipH->id = 0;
	ipH->frag_off = htons(IP_DF);
	ipH->ttl = 64;
	ipH->protocol = IPPROTO_UDP;
	ipH->saddr = inet_addr(srcIP);
	ipH->daddr = inet_addr(dstIP);
	ipH->check = 0;

	udphdr* udpH = (udphdr*)(packet_buffer + ether_len + ip_default_len);
	memset(udpH, 0, sizeof(udpH));
	udpH->uh_sport = htons(srcPort);
	udpH->uh_dport = htons(dstPort);
	udpH->uh_ulen = htons(udp_default_len + pageloadSize);

	uint8_t* pkt_data = (packet_buffer + ether_len + ip_default_len + udp_default_len);
	read(randFile, pkt_data, pageloadSize);

	pcap_pkthdr header;
	memset(&header, 0, sizeof(header));
	header.caplen = pktSize;
	header.len = pktSize;
	gettimeofday(&header.ts, 0);

	int genPktCount = 0;
	int curFlowSize = flowSize > 0 ? flowSize : 1;
	int genFlowCount = 0;
	while (pktCount-- > 0)
	{
		if (genPktCount%flowSize == 0)
		{
			dstPort += 1;
			udpH->uh_dport = htons(dstPort);
		}

		gettimeofday(&header.ts, 0);
		read(randFile, pkt_data, pageloadSize);

		pcap_dump((u_char*)handle_out, &header, packet_buffer);

		if (++genPktCount % 2000 == 0)
		{
			printf("have gen %d packet\n", genPktCount);
		}
	}
	/* And close the session */
	pcap_dump_close(handle_out);
	pcap_close(handle);
	close(randFile);

	printf("have gen %d packet\n", genPktCount);
}


/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
	int id;
	const char* path;
	int count;
	int dedup;


	if (argc == 5)
	{
		sscanf(argv[1], "%d", &id);
		path = argv[2];
		sscanf(argv[3], "%d", &count);
		sscanf(argv[4], "%d", &dedup);

		if (id == -1)
		{
			removeSameData(2, "./data/patternMatching/m57.pcap", "./data/patternMatching/m57-filter.pcap");
			return 0;
		}

		if (id == -2)
		{
			int flowSize = 10;
			int batchSize = 1024* flowSize;
			int batchCount = 10+1;
			pktGenerator("./pkts/200.pcap", batchSize*batchCount , flowSize, 200);
			pktGenerator("./pkts/400.pcap", batchSize*batchCount , flowSize, 400);
			pktGenerator("./pkts/600.pcap", batchSize*batchCount , flowSize, 600);
			pktGenerator("./pkts/800.pcap", batchSize*batchCount , flowSize, 800);
			pktGenerator("./pkts/1000.pcap", batchSize*batchCount, flowSize, 1000);
			pktGenerator("./pkts/1200.pcap", batchSize*batchCount, flowSize, 1200);
			return 0;
		}

		/* Initialize the enclave */
		if (initialize_enclave() < 0) {
			printf("Enter a character before exit ...\n");
			getchar();
			return -1;
		}

		init_server();

		// main loop
		run_server(id, path, count, dedup);

		/* Destroy the enclave */
		clear_server();
		destroy_enlave();

	}
	else
	{
		printf("Usage : dedupApplication <applicationID> <Path> <Count> <Dedup>\n");
		printf("       <applicationID> : 1:BackupFolder, 2:MiddleboxIDS, 3:MapreduceBow, 4:PicFolderDiff.\n");
		printf("       <Path>          : The input data path.\n");
		printf("       <Count>         : The input data count.\n");
		printf("       <Dedup>         : 0: disable deduplication  1: enable deduplication.\n");
		printf("\n");
		printf("  Applications 1. BackupFolder: use libz to compress the folder <Path>.\n");
		printf("               2. MiddleboxIDS: do IDS for pcap file <Path>, with <Count> packages.\n");
		printf("               3. MapreduceBow: use mapreduce to compute bow for <Count> files in <Path>.\n");
		printf("               4. PicFeature: use sift to compute pic feature in folder <Path>.\n");
		printf("\n");
	}
	return 0;
}

