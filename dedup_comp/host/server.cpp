#include "Enclave_u.h"

#include "../../common/data_type.h"
#include "../../common/network/network.h"

#include <stdio.h>

extern sgx_enclave_id_t global_eid;

Network requester(SERV_IP);

void init_server() {
    // Do whatever initializaiton
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
    int req_size = TYPE_SIZE + TAG_SIZE + sizeof(int);

    if (req_size <= TX_BUFFER_SIZE) {
        requester.set_send_buffer(&Request::Get, TYPE_SIZE, 0);
        requester.set_send_buffer(tag, TAG_SIZE, TYPE_SIZE);
        requester.set_send_buffer((uint8_t *)(&expt_size), sizeof(int), TYPE_SIZE + TAG_SIZE);

        Request req(requester.get_send_buffer(), req_size);

        requester.send_request(&req);

        Response* resp = requester.recv_response();

        if (resp) {
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
    int req_size = TAG_SIZE + sizeof(metadata) + rlt_size;

    if(req_size <= TX_BUFFER_SIZE) {
        requester.set_send_buffer(&Request::Put, TYPE_SIZE, 0);
        requester.set_send_buffer(tag, TAG_SIZE, TYPE_SIZE);
        requester.set_send_buffer(meta, sizeof(metadata), TYPE_SIZE + TAG_SIZE);
        requester.set_send_buffer(rlt, rlt_size, TYPE_SIZE + TAG_SIZE + sizeof(metadata));

        Request req(requester.get_send_buffer(), req_size);

        // send out request
        requester.send_request(&req);

        // no need to wait for response since the put is made asyncrounous, 
        // immediately return
        // TODO checking put result
    }
    else {
        printf("[*] The put request is oversized!\n");
    }
}