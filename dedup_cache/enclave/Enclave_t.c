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

static sgx_status_t SGX_CDECL sgx_ecall_cache_get(void* pms)
{
	ms_ecall_cache_get_t* ms = SGX_CAST(ms_ecall_cache_get_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	uint8_t* _tmp_tag = ms->ms_tag;
	size_t _len_tag = 32;
	uint8_t* _in_tag = NULL;
	uint8_t* _tmp_meta = ms->ms_meta;
	size_t _len_meta = 48;
	uint8_t* _in_meta = NULL;
	uint8_t* _tmp_rlt = ms->ms_rlt;
	int _tmp_expt_size = ms->ms_expt_size;
	size_t _len_rlt = _tmp_expt_size;
	uint8_t* _in_rlt = NULL;
	int* _tmp_true_size = ms->ms_true_size;
	size_t _len_true_size = 4;
	int* _in_true_size = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_cache_get_t));
	CHECK_UNIQUE_POINTER(_tmp_tag, _len_tag);
	CHECK_UNIQUE_POINTER(_tmp_meta, _len_meta);
	CHECK_UNIQUE_POINTER(_tmp_rlt, _len_rlt);
	CHECK_UNIQUE_POINTER(_tmp_true_size, _len_true_size);

	if (_tmp_tag != NULL) {
		_in_tag = (uint8_t*)malloc(_len_tag);
		if (_in_tag == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy((void*)_in_tag, _tmp_tag, _len_tag);
	}
	if (_tmp_meta != NULL) {
		if ((_in_meta = (uint8_t*)malloc(_len_meta)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_meta, 0, _len_meta);
	}
	if (_tmp_rlt != NULL) {
		if ((_in_rlt = (uint8_t*)malloc(_len_rlt)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_rlt, 0, _len_rlt);
	}
	if (_tmp_true_size != NULL) {
		if ((_in_true_size = (int*)malloc(_len_true_size)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_true_size, 0, _len_true_size);
	}
	ecall_cache_get((const uint8_t*)_in_tag, _in_meta, _in_rlt, _tmp_expt_size, _in_true_size);
err:
	if (_in_tag) free((void*)_in_tag);
	if (_in_meta) {
		memcpy(_tmp_meta, _in_meta, _len_meta);
		free(_in_meta);
	}
	if (_in_rlt) {
		memcpy(_tmp_rlt, _in_rlt, _len_rlt);
		free(_in_rlt);
	}
	if (_in_true_size) {
		memcpy(_tmp_true_size, _in_true_size, _len_true_size);
		free(_in_true_size);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_cache_put(void* pms)
{
	ms_ecall_cache_put_t* ms = SGX_CAST(ms_ecall_cache_put_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	uint8_t* _tmp_tag = ms->ms_tag;
	size_t _len_tag = 32;
	uint8_t* _in_tag = NULL;
	uint8_t* _tmp_meta = ms->ms_meta;
	size_t _len_meta = 48;
	uint8_t* _in_meta = NULL;
	uint8_t* _tmp_rlt = ms->ms_rlt;
	int _tmp_rlt_size = ms->ms_rlt_size;
	size_t _len_rlt = _tmp_rlt_size;
	uint8_t* _in_rlt = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_cache_put_t));
	CHECK_UNIQUE_POINTER(_tmp_tag, _len_tag);
	CHECK_UNIQUE_POINTER(_tmp_meta, _len_meta);
	CHECK_UNIQUE_POINTER(_tmp_rlt, _len_rlt);

	if (_tmp_tag != NULL) {
		_in_tag = (uint8_t*)malloc(_len_tag);
		if (_in_tag == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy((void*)_in_tag, _tmp_tag, _len_tag);
	}
	if (_tmp_meta != NULL) {
		_in_meta = (uint8_t*)malloc(_len_meta);
		if (_in_meta == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy((void*)_in_meta, _tmp_meta, _len_meta);
	}
	if (_tmp_rlt != NULL) {
		_in_rlt = (uint8_t*)malloc(_len_rlt);
		if (_in_rlt == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy((void*)_in_rlt, _tmp_rlt, _len_rlt);
	}
	ecall_cache_put((const uint8_t*)_in_tag, (const uint8_t*)_in_meta, (const uint8_t*)_in_rlt, _tmp_rlt_size);
err:
	if (_in_tag) free((void*)_in_tag);
	if (_in_meta) free((void*)_in_meta);
	if (_in_rlt) free((void*)_in_rlt);

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

