#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)


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

static sgx_status_t SGX_CDECL sgx_ecall_cache_get(void* pms)
{
	ms_ecall_cache_get_t* ms = SGX_CAST(ms_ecall_cache_get_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_tag = ms->ms_tag;
	size_t _len_tag = 256;
	char* _in_tag = NULL;
	char* _tmp_rlt = ms->ms_rlt;
	int _tmp_rlt_size = ms->ms_rlt_size;
	size_t _len_rlt = _tmp_rlt_size;
	char* _in_rlt = NULL;
	char* _tmp_r = ms->ms_r;
	size_t _len_r = 256;
	char* _in_r = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_cache_get_t));
	CHECK_UNIQUE_POINTER(_tmp_tag, _len_tag);
	CHECK_UNIQUE_POINTER(_tmp_rlt, _len_rlt);
	CHECK_UNIQUE_POINTER(_tmp_r, _len_r);

	if (_tmp_tag != NULL) {
		_in_tag = (char*)malloc(_len_tag);
		if (_in_tag == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_tag, _tmp_tag, _len_tag);
	}
	if (_tmp_rlt != NULL) {
		if ((_in_rlt = (char*)malloc(_len_rlt)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_rlt, 0, _len_rlt);
	}
	if (_tmp_r != NULL) {
		if ((_in_r = (char*)malloc(_len_r)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_r, 0, _len_r);
	}
	ms->ms_retval = ecall_cache_get(_in_tag, _in_rlt, _tmp_rlt_size, _in_r);
err:
	if (_in_tag) free(_in_tag);
	if (_in_rlt) {
		memcpy(_tmp_rlt, _in_rlt, _len_rlt);
		free(_in_rlt);
	}
	if (_in_r) {
		memcpy(_tmp_r, _in_r, _len_r);
		free(_in_r);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_cache_put(void* pms)
{
	ms_ecall_cache_put_t* ms = SGX_CAST(ms_ecall_cache_put_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_tag = ms->ms_tag;
	size_t _len_tag = 256;
	char* _in_tag = NULL;
	char* _tmp_rlt = ms->ms_rlt;
	int _tmp_rlt_size = ms->ms_rlt_size;
	size_t _len_rlt = _tmp_rlt_size;
	char* _in_rlt = NULL;
	char* _tmp_r = ms->ms_r;
	size_t _len_r = 256;
	char* _in_r = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_cache_put_t));
	CHECK_UNIQUE_POINTER(_tmp_tag, _len_tag);
	CHECK_UNIQUE_POINTER(_tmp_rlt, _len_rlt);
	CHECK_UNIQUE_POINTER(_tmp_r, _len_r);

	if (_tmp_tag != NULL) {
		_in_tag = (char*)malloc(_len_tag);
		if (_in_tag == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_tag, _tmp_tag, _len_tag);
	}
	if (_tmp_rlt != NULL) {
		_in_rlt = (char*)malloc(_len_rlt);
		if (_in_rlt == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_rlt, _tmp_rlt, _len_rlt);
	}
	if (_tmp_r != NULL) {
		_in_r = (char*)malloc(_len_r);
		if (_in_r == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_r, _tmp_r, _len_r);
	}
	ecall_cache_put(_in_tag, _in_rlt, _tmp_rlt_size, _in_r);
err:
	if (_in_tag) free(_in_tag);
	if (_in_rlt) free(_in_rlt);
	if (_in_r) free(_in_r);

	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[2];
} g_ecall_table = {
	2,
	{
		{(void*)(uintptr_t)sgx_ecall_cache_get, 0},
		{(void*)(uintptr_t)sgx_ecall_cache_put, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][2];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_print_string(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_string_t);
	void *__tmp = NULL;

	ocalloc_size += (str != NULL && sgx_is_within_enclave(str, _len_str)) ? _len_str : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_string_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_string_t));

	if (str != NULL && sgx_is_within_enclave(str, _len_str)) {
		ms->ms_str = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_str);
		memcpy((void*)ms->ms_str, str, _len_str);
	} else if (str == NULL) {
		ms->ms_str = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0, ms);


	sgx_ocfree();
	return status;
}

