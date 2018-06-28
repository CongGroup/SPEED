#include "Enclave_u.h"


#include "sgx_urts.h"
#include <memory>
#include <signal.h>

extern sgx_enclave_id_t global_eid;

void shutdown(int sig) {
    printf("[*] Stopping caching server ...\n");

    sgx_destroy_enclave(global_eid);

    // Do whatever tear-down
	abort();
}

void init_server() {
    // Do whatever initializaiton
    //printf("[*] Initializing computing server ...\n");

    signal(SIGTERM, shutdown);
    signal(SIGQUIT, shutdown);
    signal(SIGTSTP, shutdown); // Ctrl + z
    signal(SIGINT, shutdown); // Ctrl + c

    //printf("[*] Connecting to caching server ...\n");

    printf("[*] Initialization is finished!\n");

}

void run_server(int id, const char* path, int count) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_entrance(global_eid, id, path, count);
    if (ret != SGX_SUCCESS) {
        printf("[*] Fail to run computing!\n");
    }
}

