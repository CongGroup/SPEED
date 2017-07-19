#include "server.h"

#include "Enclave_u.h"
#include "sgx_urts.h"

#include "../../common/data_type.h"
#include "../../common/network/network.h"

#include <assert.h>
#include <memory>
#include <signal.h>
#include <stdio.h>

extern sgx_enclave_id_t global_eid;

std::auto_ptr<Network> responser;

void shutdown(int sig) {
    printf("[*] Stopping caching server ...\n");

    sgx_destroy_enclave(global_eid);

    responser.release();

	abort();

    // Do whatever tear-down
}

void init_server() {
    // Do whatever initialization
    printf("[*] Initializing caching server ...\n");

    signal(SIGTERM, shutdown);
    signal(SIGQUIT, shutdown);
    signal(SIGTSTP, shutdown); // Ctrl + z
    signal(SIGINT, shutdown); // Ctrl + c

    printf("[*] Connecting to computing server ...\n");
    responser.reset(new Network(0));

    printf("[*] Initialization is finished!\n");
}

Response *parse_request_get(const Request *req)
{
    int true_size = 0;
    uint8_t *buffer = responser->get_send_buffer();

    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_cache_get(global_eid, req->get_tag(),
                          buffer + RESP_TYPE_SIZE, // meta
                          buffer + RESP_TYPE_SIZE + sizeof(metadata),  req->get_expt_rlt_size(),
                          &true_size);

    if (ret != SGX_SUCCESS) {
        printf("***Enclave error***");
        return NULL;
    }
    else if (true_size == 0) {
        responser->set_send_buffer(&Response::Negative, RESP_TYPE_SIZE, 0);
        Response *resp = new Response(buffer, RESP_TYPE_SIZE);
        return resp;
    }
    else {
        int resp_size = RESP_TYPE_SIZE + sizeof(metadata) + std::min(true_size, req->get_expt_rlt_size());
        responser->set_send_buffer(&Response::Positive, RESP_TYPE_SIZE, 0);
        Response *resp = new Response(buffer, resp_size);
        return resp;
    }
}

Response *parse_request_put(const Request *req)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_cache_put(global_eid, req->get_tag(), req->get_meta(),
                                      req->get_rlt(), req->get_rlt_size());

    if (ret != SGX_SUCCESS) {
        printf("***Enclave error***");
        return NULL;
    }
    else {
        // TODO design put response
        return NULL;
    }
}

void run_server() {
    std::auto_ptr<Request> req;
    std::auto_ptr<Response> resp;

    /*int size = responser->recv_msg(responser->responser);
    char *buffer = (char *)responser->m_recv_buffer;
    *(buffer + size) = 0;
    printf("%d %s\n", size, buffer);*/

    printf("[*] Waiting for request ...\n");
    while (true) {
        // block until timeout (20s)
        req.reset(responser->recv_request());

        if (req.get()) {
            switch (req->get_type()) 
			{
                case 1: //Request::Get:
                    printf("[*] Get request received --> ");
                    resp.reset(parse_request_get(req.get()));
                    if (resp.get())
                        printf("%d bytes retrived!\n", resp->get_rlt_size());
                    else
                        printf("missed in cache!\n");
                    break;
                case 2: //Request::Put:
                    printf("[*] Put request received --> ");
                    resp.reset(parse_request_put(req.get()));
                    printf("%d bytes cached!\n", req->get_rlt_size());
                    break;
                default:
                    assert(false);
            }
            
            // send out response
            if (resp.get()) {
                responser->send_response(resp.get());
            }
        }
		else
		{
			printf("Null request,exit. \n");
			break;
		}
    }
}