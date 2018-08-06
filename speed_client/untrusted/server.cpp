#include "Enclave_u.h"


#include "sgx_urts.h"
#include <memory>
#include <signal.h>
#include "../../common/network/network.h"
#include <redis3m/redis3m.hpp>
#include "server.h"

extern sgx_enclave_id_t global_eid;

extern std::auto_ptr<Network> requester;

extern redis3m::connection::ptr_t m_ptrConnection;

void shutdown(int sig) {
    printf("[*] Stopping caching server ...\n");

    sgx_destroy_enclave(global_eid);

#ifndef USE_LOCAL_CACHE
	requester.release();
#else
	m_ptrConnection.reset((redis3m::connection*)0);
#endif // !USE_LOCAL_CACHE

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

#ifndef USE_LOCAL_CACHE
	printf("[*] Connecting to caching server ...\n");
	requester.reset(new Network(SERV_IP));
#endif // !USE_LOCAL_CACHE

    printf("[*] Initialization is finished!\n");

}

void run_server(int id, const char* path, int count, int dedup) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_entrance(global_eid, id, path, count, dedup);
    if (ret != SGX_SUCCESS) {
        printf("[*] Fail to run computing!\n");
    }
}

void clear_server()
{

#ifndef USE_LOCAL_CACHE
	requester.release();
#else
	m_ptrConnection.reset((redis3m::connection*)0);
#endif // !USE_LOCAL_CACHE


}

