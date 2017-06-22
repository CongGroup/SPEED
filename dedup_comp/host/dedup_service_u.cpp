#include "Enclave_u.h"

#include <stdint.h>

void ocall_request_find(const uint8_t *tag,
                        int *resp_size, 
                        uint8_t *rlt, int exp_size,
                        uint8_t *meta)
{
    // prepare request

    // send out request

    // wait for response

    // parse response

    // copy responsed data to rlt and r
    // dummy for simulation
    *resp_size = 0;
    memset(rlt, 1, exp_size);
}

void ocall_request_put(const uint8_t *tag,
                       const uint8_t *rlt, int rlt_size,
                       const uint8_t *meta)
{
    // prepare request

    // send out request

    // no need to wait for response since the put is made asyncrounous, 
    // immediately return
}