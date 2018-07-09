#include "enclave_management.h"
#include "server.h"

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
		destroy_enlave();

	}
	else
	{
		printf("Usage : dedupApplication <applicationID> <Path> <Count>\n");
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

