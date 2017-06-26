#include "Enclave_u.h"
#include <errno.h>


typedef struct ms_ocall_print_string_t {
	char* ms_str;
} ms_ocall_print_string_t;

typedef struct ms_ocall_load_text_file_t {
	char* ms_filename;
	char* ms_buffer;
	int ms_buffer_size;
	int* ms_filesize;
} ms_ocall_load_text_file_t;

typedef struct ms_ocall_get_time_t {
	long int* ms_second;
	long int* ms_nanosecond;
} ms_ocall_get_time_t;

typedef struct ms_ocall_request_find_t {
	uint8_t* ms_tag;
	uint8_t* ms_meta;
	uint8_t* ms_rlt;
	int ms_expt_size;
	int* ms_true_size;
} ms_ocall_request_find_t;

typedef struct ms_ocall_request_put_t {
	uint8_t* ms_tag;
	uint8_t* ms_meta;
	uint8_t* ms_rlt;
	int ms_rlt_size;
} ms_ocall_request_put_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_load_text_file(void* pms)
{
	ms_ocall_load_text_file_t* ms = SGX_CAST(ms_ocall_load_text_file_t*, pms);
	ocall_load_text_file((const char*)ms->ms_filename, ms->ms_buffer, ms->ms_buffer_size, ms->ms_filesize);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_get_time(void* pms)
{
	ms_ocall_get_time_t* ms = SGX_CAST(ms_ocall_get_time_t*, pms);
	ocall_get_time(ms->ms_second, ms->ms_nanosecond);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_request_find(void* pms)
{
	ms_ocall_request_find_t* ms = SGX_CAST(ms_ocall_request_find_t*, pms);
	ocall_request_find((const uint8_t*)ms->ms_tag, ms->ms_meta, ms->ms_rlt, ms->ms_expt_size, ms->ms_true_size);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_request_put(void* pms)
{
	ms_ocall_request_put_t* ms = SGX_CAST(ms_ocall_request_put_t*, pms);
	ocall_request_put((const uint8_t*)ms->ms_tag, (const uint8_t*)ms->ms_meta, (const uint8_t*)ms->ms_rlt, ms->ms_rlt_size);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[5];
} ocall_table_Enclave = {
	5,
	{
		(void*)Enclave_ocall_print_string,
		(void*)Enclave_ocall_load_text_file,
		(void*)Enclave_ocall_get_time,
		(void*)Enclave_ocall_request_find,
		(void*)Enclave_ocall_request_put,
	}
};
sgx_status_t ecall_entrance(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, NULL);
	return status;
}

