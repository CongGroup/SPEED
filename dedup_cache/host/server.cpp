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
		buffer + RESP_TYPE_SIZE + sizeof(metadata), req->get_expt_rlt_size(),
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

#ifndef TEST_PUT_AND_GET

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

#else

#include <cstring>
#include <map>
#include <vector>
#include <utility> // pair
const int dataSizes[] = { 1024 * 1, 1024 * 1024, 1024 * 100, 1024 * 10, 1024 * 1 };
#define ROUND 100
#define HIT true
//#define DATASIZE (1024 * 1)

typedef std::vector<uint8_t> binary;
typedef std::pair<binary, binary> entry_t;
typedef std::map<binary, entry_t> cache_t;



char* nextTag(bool clear)
{
	static byte tag[TAG_SIZE] = { 0 };

	if (!clear || !HIT)
	{
		(*(long*)tag)++;
	}
	else
	{
		memset(tag, 0, sizeof(tag));
	}

	return (char*)tag;
}

static void get_time(hrtime *time)
{
	ocall_get_time(&time->second, &time->nanosecond);
}


void run_server()
{
	Request* PutRequests[ROUND];
	Request* GetRequests[ROUND];

	for (int nn = 0; nn < sizeof(dataSizes) / sizeof(int); nn++)
	{

		printf("DataSize is %d bytes.\n", dataSizes[nn]);

		byte buffer[REQ_TYPE_SIZE + TAG_SIZE + sizeof(metadata) + dataSizes[nn] + sizeof(int)];
		byte data[dataSizes[nn]];
		memset(data, 0, dataSizes[nn]);
		metadata meta;
		int expSize = dataSizes[nn];

		expSize = 4;
		//prepare Request
		for (int i = 0; i < ROUND; i++)
		{
			int req_size;

			req_size = REQ_TYPE_SIZE + TAG_SIZE + sizeof(int);
			memcpy(buffer, &Request::Get, REQ_TYPE_SIZE);
			memcpy(buffer + REQ_TYPE_SIZE, nextTag(false), TAG_SIZE);
			memcpy(buffer + REQ_TYPE_SIZE + TAG_SIZE, &expSize, sizeof(expSize));
			GetRequests[i] = new Request(buffer, req_size);

			nextTag(true);

			req_size = REQ_TYPE_SIZE + TAG_SIZE + sizeof(metadata) + expSize;
			memcpy(buffer, &Request::Put, REQ_TYPE_SIZE);
			memcpy(buffer + REQ_TYPE_SIZE, nextTag(false), TAG_SIZE);
			memcpy(buffer + REQ_TYPE_SIZE + TAG_SIZE, &meta, sizeof(metadata));
			memcpy(buffer + REQ_TYPE_SIZE + TAG_SIZE + sizeof(metadata), &expSize, expSize);
			PutRequests[i] = new Request(buffer, req_size);
		}

		hrtime start_time, end_time;

		get_time(&start_time);
		for (int i = 0; i < ROUND; i++)
		{
			parse_request_put(PutRequests[i]);
			memcpy(data, buffer, dataSizes[nn]);
		}
		get_time(&end_time);
		printf("In SGX put %d round use %d us.\n", ROUND, time_elapsed_in_us(&start_time, &end_time));

		get_time(&start_time);
		for (int i = 0; i < ROUND; i++)
		{
			parse_request_get(GetRequests[i]);
			memcpy(data, buffer, dataSizes[nn]);
		}
		get_time(&end_time);
		printf("In SGX get %d round use %d us.\n", ROUND, time_elapsed_in_us(&start_time, &end_time));

		cache_t cache;

		expSize = dataSizes[nn];

		nextTag(true);
		get_time(&start_time);
		for (int i = 0; i < ROUND; i++)
		{
			binary key(TAG_SIZE, 0);
			memcpy(&key[0], nextTag(false), TAG_SIZE);

			binary entry_meta(sizeof(metadata), 0);
			memcpy(&entry_meta[0], &meta, sizeof(metadata));

			//binary entry_rlt(expSize, 0);
			memcpy(data, buffer, expSize);

			cache[key] = entry_t(entry_meta, entry_meta);
			//cache[key] = entry_t(entry_meta, entry_rlt);
		}
		get_time(&end_time);
		printf("Out SGX put %d round use %d us.\n", ROUND, time_elapsed_in_us(&start_time, &end_time));


		nextTag(true);
		get_time(&start_time);
		for (int i = 0; i < ROUND; i++)
		{
			binary key(TAG_SIZE, 0);
			memcpy(&key[0], nextTag(false), TAG_SIZE);

			cache_t::const_iterator it = cache.find(key);
			if (it != cache.end())
			{
				const entry_t &entry = it->second;

				// copy meta data
				memcpy(&meta, &entry.first[0], sizeof(meta));

				// copy computation result
				//expSize = entry.second.size();
				memcpy(data, buffer, expSize);
			}
		}
		get_time(&end_time);
		printf("Out SGX get %d round use %d us.\n", ROUND, time_elapsed_in_us(&start_time, &end_time));

		for (int i = 0; i < ROUND; i++)
		{
			delete PutRequests[i];
			delete GetRequests[i];
		}
	}
}

#endif