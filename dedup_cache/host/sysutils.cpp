//#include "sgx_urts.h"
#include "Enclave_u.h"
#include <stdio.h>

///* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
    * the input string to prevent buffer overflow.
    */
    printf("%s", str);
}