#include "enclave_management.h"
#include "server.h"

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
	int id;
	const char* path;
	int count;


	if (argc == 4)
	{
		sscanf(argv[1], "%d", &id);
		path = argv[2];
		sscanf(argv[3], "%d", &count);
		

		/* Initialize the enclave */
		if (initialize_enclave() < 0) {
			printf("Enter a character before exit ...\n");
			getchar();
			return -1;
		}



		init_server();

		// main loop
		run_server(id, path, count);

		/* Destroy the enclave */
		destroy_enlave();

	}
	else
	{
		printf("Usage : dedupApplication <applicationID> <Path> <Count>\n");
		printf("       <applicationID> : 1:BackupFolder, 2:MiddleboxIDS, 3:MapreduceBow, 4:PicFolderDiff.\n");
		printf("       <Path>          : The input data path.\n");
		printf("       <Count>         : The input data count.\n");
		printf("\n");
		printf("  Applications 1. BackupFolder: use libz to compress the folder <Path>.\n");
		printf("               2. MiddleboxIDS: do IDS for pcap file <Path>, with <Count> packages.\n");
		printf("               3. MapreduceBow: use mapreduce to compute bow for <Count> files in <Path>.\n");
		printf("               4. PicFolderDiff: use sift to fild similar pic in folder <Path>.\n");
	}
	return 0;
}

