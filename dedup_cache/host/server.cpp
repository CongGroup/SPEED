#include "server.h"

#include "Enclave_u.h"
#include "sgx_urts.h"

#include "../../common/network/network.h"

#include <stdio.h>

extern sgx_enclave_id_t global_eid;

void init_server() {
    // Do whatever initialization
}

void test_cache() {
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
}

void run_server() {
    Network responser(0);
    Request *req;

    while (true) {
        req = responser.recv_request();
        
        // make ECALL ...

        // prepare response

        // send out response
    }
}