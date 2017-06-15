#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_cache_get_t {
	int ms_retval;
	char* ms_tag;
	char* ms_rlt;
	int ms_rlt_size;
	char* ms_r;
} ms_ecall_cache_get_t;

typedef struct ms_ecall_cache_put_t {
	char* ms_tag;
	char* ms_rlt;
	int ms_rlt_size;
	char* ms_r;
} ms_ecall_cache_put_t;

typedef struct ms_ocall_print_string_t {
	char* ms_str;
} ms_ocall_print_string_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave = {
	1,
	{
		(void*)Enclave_ocall_print_string,
	}
};
sgx_status_t ecall_cache_get(sgx_enclave_id_t eid, int* retval, char* tag, char* rlt, int rlt_size, char* r)
{
	sgx_status_t status;
	ms_ecall_cache_get_t ms;
	ms.ms_tag = tag;
	ms.ms_rlt = rlt;
	ms.ms_rlt_size = rlt_size;
	ms.ms_r = r;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_cache_put(sgx_enclave_id_t eid, char* tag, char* rlt, int rlt_size, char* r)
{
	sgx_status_t status;
	ms_ecall_cache_put_t ms;
	ms.ms_tag = tag;
	ms.ms_rlt = rlt;
	ms.ms_rlt_size = rlt_size;
	ms.ms_r = r;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

