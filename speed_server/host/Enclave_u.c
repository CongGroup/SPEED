#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_cache_get_t {
	uint8_t* ms_tag;
	uint8_t* ms_meta;
	uint8_t* ms_rlt;
	int ms_expt_size;
	int* ms_true_size;
} ms_ecall_cache_get_t;

typedef struct ms_ecall_cache_put_t {
	uint8_t* ms_tag;
	uint8_t* ms_meta;
	uint8_t* ms_rlt;
	int ms_rlt_size;
} ms_ecall_cache_put_t;

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
	int* ms_second;
	int* ms_nanosecond;
} ms_ocall_get_time_t;

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

static const struct {
	size_t nr_ocall;
	void * table[3];
} ocall_table_Enclave = {
	3,
	{
		(void*)Enclave_ocall_print_string,
		(void*)Enclave_ocall_load_text_file,
		(void*)Enclave_ocall_get_time,
	}
};
sgx_status_t ecall_cache_get(sgx_enclave_id_t eid, const uint8_t* tag, uint8_t* meta, uint8_t* rlt, int expt_size, int* true_size)
{
	sgx_status_t status;
	ms_ecall_cache_get_t ms;
	ms.ms_tag = (uint8_t*)tag;
	ms.ms_meta = meta;
	ms.ms_rlt = rlt;
	ms.ms_expt_size = expt_size;
	ms.ms_true_size = true_size;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_cache_put(sgx_enclave_id_t eid, const uint8_t* tag, const uint8_t* meta, const uint8_t* rlt, int rlt_size)
{
	sgx_status_t status;
	ms_ecall_cache_put_t ms;
	ms.ms_tag = (uint8_t*)tag;
	ms.ms_meta = (uint8_t*)meta;
	ms.ms_rlt = (uint8_t*)rlt;
	ms.ms_rlt_size = rlt_size;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

