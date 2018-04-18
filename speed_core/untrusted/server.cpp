#include "Enclave_u.h"

#include "../common/data_type.h"

#include "sgx_urts.h"

#include <memory>
#include <signal.h>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>
#include <stdint.h>
#include <time.h>

typedef std::vector<uint8_t> binary;
typedef std::pair<binary, binary> entry_t;
// TODO: replace with unordered_map
// We need to know map max size to use unordered_map
//typedef std::unordered_map<binary, entry_t> cache_t;
typedef std::map<binary, entry_t> cache_t;

extern sgx_enclave_id_t global_eid;


void shutdown(int sig) {
    printf("[*] Stopping caching server ...\n");

    sgx_destroy_enclave(global_eid);



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

    printf("[*] Initialization is finished!\n");

}

void run_server() {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = ecall_entrance(global_eid);
    if (ret != SGX_SUCCESS) {
        printf("[*] Fail to launch computing server!\n");
    }
}

cache_t cache;

void ocall_request_find(const uint8_t *tag,
	uint8_t *meta,
	uint8_t *rlt, int expt_size,
	int *true_size)
{
	binary key(TAG_SIZE, 0);
	memcpy(&key[0], tag, TAG_SIZE);

	cache_t::const_iterator it = cache.find(key);

	if (it == cache.end()) {
		*true_size = 0;
	}
	else {
		const entry_t &entry = it->second;
		*true_size = entry.second.size();

		memcpy(meta, &entry.first[0], entry.first.size());

		memcpy(rlt, &entry.second[0], std::min(*true_size, expt_size));
	}
}

void ocall_request_put(const uint8_t *tag,
	const uint8_t *meta,
	const uint8_t *rlt, int rlt_size)
{
	binary key(TAG_SIZE, 0);
	memcpy(&key[0], tag, TAG_SIZE);

	binary entry_meta(sizeof(metadata), 0);
	memcpy(&entry_meta[0], meta, sizeof(metadata));

	binary entry_rlt(rlt_size, 0);
	memcpy(&entry_rlt[0], rlt, rlt_size);

	cache[key] = entry_t(entry_meta, entry_rlt);
}



