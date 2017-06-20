//#include "sgx_urts.h"
#include "Enclave_u.h"
#include <stdio.h>
#include <time.h>

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
    * the input string to prevent buffer overflow.
    */
    printf("%s", str);
}

void ocall_get_time(long *second, long *nanosecond)
{
    timespec wall_clock;
    clock_gettime(CLOCK_REALTIME, &wall_clock);
    *second = wall_clock.tv_sec;
    *nanosecond = wall_clock.tv_nsec;
}