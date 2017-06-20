#include "server.h"

#include "Enclave_u.h"
#include "sgx_urts.h"

#include <iostream>

extern sgx_enclave_id_t global_eid;

void init_server() {
    // remember to register signal handler
}

void run_server() {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_entrance(global_eid);
    if (ret != SGX_SUCCESS) {
        ;
    }
}