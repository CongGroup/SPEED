#include "Enclave_u.h"

#include "../../common/data_type.h"
#include "../../common/network/network.h"

#include "sgx_urts.h"

#include <memory>
#include <signal.h>
#include <stdio.h>
#include <sstream>

extern sgx_enclave_id_t global_eid;

std::auto_ptr<Network> requester;

void shutdown(int sig) {
    printf("[*] Stopping caching server ...\n");

    sgx_destroy_enclave(global_eid);

    requester.release();

    // Do whatever tear-down
	abort();
}

void init_server() {
    // Do whatever initializaiton
    printf("[*] Initializing computing server ...\n");

    signal(SIGTERM, shutdown);
    signal(SIGQUIT, shutdown);
    signal(SIGTSTP, shutdown); // Ctrl + z
    signal(SIGINT, shutdown); // Ctrl + c

    printf("[*] Connecting to caching server ...\n");
    requester.reset(new Network(SERV_IP));

    printf("[*] Initialization is finished!\n");

}

void run_server() {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_entrance(global_eid);
    if (ret != SGX_SUCCESS) {
        printf("[*] Fail to launch computing server!\n");
    }
}

void ocall_request_find(const uint8_t *tag,
                        uint8_t *meta,
                        uint8_t *rlt, int expt_size,
                        int *true_size)
{
    int req_size = REQ_TYPE_SIZE + TAG_SIZE + sizeof(int);

    if (req_size <= TX_BUFFER_SIZE) {
        requester->set_send_buffer(&Request::Get, REQ_TYPE_SIZE, 0);
        requester->set_send_buffer(tag, TAG_SIZE, REQ_TYPE_SIZE);
        requester->set_send_buffer((uint8_t *)(&expt_size), sizeof(int), REQ_TYPE_SIZE + TAG_SIZE);

        Request req(requester->get_send_buffer(), req_size);

        requester->send_request(&req);

        std::auto_ptr<Response> resp(requester->recv_response());

        if (resp->get_answer() == Response::Positive) {
            // copy back responsed data
            memcpy(meta, resp->get_meta(), sizeof(metadata));
            memcpy(rlt, resp->get_rlt(), resp->get_rlt_size());
            *true_size = resp->get_rlt_size();
        }
        else {
            *true_size = 0;
        }
    }
    else {
        printf("[*] The get request is oversized!\n");
    }
}

void ocall_request_put(const uint8_t *tag,
                       const uint8_t *meta,
                       const uint8_t *rlt, int rlt_size)
{
    int req_size = REQ_TYPE_SIZE + TAG_SIZE + sizeof(metadata) + rlt_size;

    if(req_size <= TX_BUFFER_SIZE) {
        requester->set_send_buffer(&Request::Put, REQ_TYPE_SIZE, 0);
        requester->set_send_buffer(tag, TAG_SIZE, REQ_TYPE_SIZE);
        requester->set_send_buffer(meta, sizeof(metadata), REQ_TYPE_SIZE + TAG_SIZE);
        requester->set_send_buffer(rlt, rlt_size, REQ_TYPE_SIZE + TAG_SIZE + sizeof(metadata));

        Request req(requester->get_send_buffer(), req_size);

        // send out request
        requester->send_request(&req);

        // no need to wait for response since the put is made asyncrounous, immediately return
    }
    else {
        printf("[*] The put request is oversized!\n");
    }
};


