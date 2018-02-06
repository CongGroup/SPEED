//#include "sgx_urts.h"
#include "Enclave_u.h"
#include <stdio.h>
#include <time.h>
#include "../../common/config.h"

#include <fstream>
#include <sstream>

#include "pgm_tool.h"

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
    * the input string to prevent buffer overflow.
    */
    printf("%s", str);
}

void ocall_get_time(int *second, int *nanosecond)
{
    timespec wall_clock;
    clock_gettime(CLOCK_REALTIME, &wall_clock);
    *second = wall_clock.tv_sec;
    *nanosecond = wall_clock.tv_nsec;
}

void ocall_load_text_file(const char *filename,
                          char **pbuffer, int buffer_size,
                          int *filesize) 
{
    // consider alternative impl to speed up loading of large file

	static char* const file_buffer(new char[FILE_BUFFER_SIZE]);

	if (strstr(filename, ".pgm") != 0)
	{
		pgm_tool pgmTool;
		pgm_tool::PgmBuffer pgm_buffer;

		std::ifstream in(filename, std::ios::binary);

		if (in.good()) {
			pgmTool.extractPgm(in, pgm_buffer);

			int dataSize = pgm_buffer.width*pgm_buffer.height * sizeof(float);

			char* databuffer = file_buffer;

			memcpy(databuffer, &pgm_buffer.width, sizeof(int));
			databuffer += sizeof(int);
			memcpy(databuffer, &pgm_buffer.height, sizeof(int));
			databuffer += sizeof(int);
			memcpy(databuffer, pgm_buffer.data, dataSize);

			*filesize = dataSize + sizeof(int)*2;

			*pbuffer = file_buffer;

			printf("read pgm file, w:%d ,h:%d ,size:%d\n", pgm_buffer.width,pgm_buffer.height,dataSize);
		}
		else {
			printf("[*] Fail to open file %s\n", filename);
			*filesize = 0;
		}
		
		in.close();

		return;
	}

	*pbuffer = file_buffer;

    std::ifstream infile(filename);
    if (infile.good()) {
        std::stringstream sstream;
        sstream << infile.rdbuf();
        std::string file = sstream.str();

        memcpy(*pbuffer, file.data(), file.size());
        *filesize = file.size();
    }
    else {
        printf("[*] Fail to open file %s\n", filename);
        *filesize = 0;
    }

    infile.close();
}


void ocall_write_text_file(const char *filename,char *buffer, int buffer_size)
{
	std::ofstream outfile(filename, std::ofstream::app);
	if (outfile.good()) {
		outfile.write("\n\n", 2);
		outfile.write(buffer, buffer_size);
	}
	else {
		printf("[*] Fail to open file %s\n", filename);
	}

	outfile.close();
}