#include "server.h"

#include "Enclave_u.h"
#include "sgx_urts.h"

#include <iostream>

extern sgx_enclave_id_t global_eid;

void init_server() {
    // remember to register signal handler
}

void run_server() {
    /*** test code ***/
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int hit = 0;
    char tag[256] = "12345";
    char rlt[] = "asdadadadqweqfsadadadasdada";
    int rlt_size = sizeof(rlt) / sizeof(rlt[0]);
    char r[256] = "asdqewq123";
    // get inexistent entry
    ret = ecall_cache_get(global_eid, &hit, tag, rlt, rlt_size, r);
    if (ret != SGX_SUCCESS) {
        printf("Fail to get cache entry!\n");
    }
    printf("Get result: %d\n", hit);
    // put the entry
    ret = ecall_cache_put(global_eid, tag, rlt, rlt_size, r);
    if (ret != SGX_SUCCESS) {
        printf("Fail to put cache entry!\n");
    }
    // get again
    ret = ecall_cache_get(global_eid, &hit, tag, rlt, rlt_size, r);
    if (ret != SGX_SUCCESS) {
        printf("Fail to get cache entry!\n");
    }
    printf("Get result: %d\n", hit);

    exit(0);
    /*** End ***/

    while (true) {
        // on receiving request (depend on how we handle networking)

        // parse request

        // make ECALL ...

        // prepare response

        // send out response
    }
}