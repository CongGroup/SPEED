#include "mr_wordcount.h"
#include "Enclave_t.h"
#include "sysutils.h"

void wordcount_map(char *file_name) 
{
	int fp;
	ocall_open(&fp, file_name, 0);

	const int buffer_size = 512;
	char buffer[buffer_size] = { 0 };
    int size = 0;
    while (1)
	{
		ocall_read(&size, fp, buffer, buffer_size - 1);
		if (size <= 0)
			break;

        char *token, *dummy = buffer;
        while ((token = strsep(&dummy, " \t\n\r")) != NULL) 
		{
			if (strlen(token) <= 0)
			{
				continue;
			}
			char *str_one = new char[2];
			str_one[0] = '1';
			str_one[1] = 0;
            MR_Emit(token, str_one);
        }
    }
	ocall_close(fp);
}

void wordcount_reduce(char *key, Getter get_next, int partition_number) 
{
    int count = 0;
    char *value;
    while ((value = get_next(key, partition_number)) != NULL)
        count++;

    eprintf("%s %d\n", key, count);
}

int wordcount_run(int argc, char *argv[]) {
    MR_Run(argc, argv, wordcount_map, 2, wordcount_reduce, 2, MR_DefaultHashPartition);
	return 0;
}
