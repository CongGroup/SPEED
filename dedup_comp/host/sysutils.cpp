//#include "sgx_urts.h"
#include "Enclave_u.h"
#include <stdio.h>
#include <time.h>

#include <fstream>
#include <sstream>

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
                          char *buffer, int buffer_size,
                          int *filesize) 
{
    // consider alternative impl to speed up loading of large file
    std::ifstream infile(filename);
    if (infile.good()) {
        std::stringstream sstream;
        sstream << infile.rdbuf();
        std::string file = sstream.str();
        
        memcpy(buffer, file.data(), file.size());
        *filesize = file.size();
    }
    else {
        printf("[*] Fail to open file %s\n", filename);
        *filesize = 0;
    }

    infile.close();
}