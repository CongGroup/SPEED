#include "server.h"

#include "Enclave_u.h"
#include "sgx_urts.h"

#include "../../common/data_type.h"
#include "../../common/network/network.h"

#include <assert.h>
#include <memory>
#include <stdio.h>

extern sgx_enclave_id_t global_eid;

Network responser(0);

void init_server() {
    // Do whatever initialization
    printf("[*] Initializing ...\n");
}

//void test_cache() {
//    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
//    int hit = 0;
//    char tag[256] = "12345";
//    char rlt[] = "asdadadadqweqfsadadadasdada";
//    int rlt_size = sizeof(rlt) / sizeof(rlt[0]);
//    char r[256] = "asdqewq123";
//    // get inexistent entry
//    ret = ecall_cache_get(global_eid, &hit, tag, rlt, rlt_size, r);
//    if (ret != SGX_SUCCESS) {
//        printf("Fail to get cache entry!\n");
//    }
//    printf("Get result: %d\n", hit);
//    // put the entry
//    ret = ecall_cache_put(global_eid, tag, rlt, rlt_size, r);
//    if (ret != SGX_SUCCESS) {
//        printf("Fail to put cache entry!\n");
//    }
//    // get again
//    ret = ecall_cache_get(global_eid, &hit, tag, rlt, rlt_size, r);
//    if (ret != SGX_SUCCESS) {
//        printf("Fail to get cache entry!\n");
//    }
//    printf("Get result: %d\n", hit);
//}

Response *parse_request_get(const Request *req)
{
    int true_size = 0;
    uint8_t *buffer = responser.get_send_buffer();

    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_cache_get(global_eid, req->get_tag(), 
                          buffer, // meta
                          buffer + sizeof(metadata),  req->get_expt_rlt_size(), // result
                          &true_size);
    if (ret != SGX_SUCCESS || true_size == 0) {
        printf("[*] Fail to get cache entry!\n");
        return NULL;
    }
    else {
        if (true_size > req->get_expt_rlt_size())
            printf("[*] The result has been truancated from %d bytes to %d bytes!\n",
                true_size, req->get_expt_rlt_size());
        Response *resp = new Response(buffer, sizeof(metadata)+req->get_expt_rlt_size());
        return resp;
    }
}

Response *parse_request_put(const Request *req)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_cache_put(global_eid, req->get_tag(), req->get_meta(),
                                      req->get_rlt(), req->get_rlt_size());
    if (ret != SGX_SUCCESS) {
        printf("[*] Fail to put cache entry!\n");
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

    printf("[*] Waiting for request ...\n");
    while (true) {
        // block until timeout (20s)
        req.reset(responser.recv_request());

        if (req.get()) {
            printf("[*] Request received! Now processing ...\n");

            switch (req->get_type()) {
            case 1: //Request::Get:
                resp.reset(parse_request_get(req.get()));
                break;
            case 2: //Request::Put:
                resp.reset(parse_request_put(req.get()));
                break;
            default:
                assert(false);
            }
            
            printf("[*] Request processed! Now sending back response ...\n");
            // send out response
            if (resp.get()) {
                responser.send_response(resp.get());
            }
        }
    }
}