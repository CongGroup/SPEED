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

static sgx_status_t SGX_CDECL sgx_ecall_cache_get(void* pms)
{
	ms_ecall_cache_get_t* ms = SGX_CAST(ms_ecall_cache_get_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	uint8_t* _tmp_tag = ms->ms_tag;
	size_t _len_tag = 32;
	uint8_t* _in_tag = NULL;
	uint8_t* _tmp_meta = ms->ms_meta;
	size_t _len_meta = 64;
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
	size_t _len_meta = 64;
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
	uint8_t entry_table[3][2];
} g_dyn_entry_table = {
	3,
	{
		{0, 0, },
		{0, 0, },
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

sgx_status_t SGX_CDECL ocall_load_text_file(const char* filename, char* buffer, int buffer_size, int* filesize)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;
	size_t _len_buffer = buffer_size;
	size_t _len_filesize = 4;

	ms_ocall_load_text_file_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_load_text_file_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;
	ocalloc_size += (buffer != NULL && sgx_is_within_enclave(buffer, _len_buffer)) ? _len_buffer : 0;
	ocalloc_size += (filesize != NULL && sgx_is_within_enclave(filesize, _len_filesize)) ? _len_filesize : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_load_text_file_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_load_text_file_t));

	if (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) {
		ms->ms_filename = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_filename);
		memcpy((void*)ms->ms_filename, filename, _len_filename);
	} else if (filename == NULL) {
		ms->ms_filename = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (buffer != NULL && sgx_is_within_enclave(buffer, _len_buffer)) {
		ms->ms_buffer = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_buffer);
		memset(ms->ms_buffer, 0, _len_buffer);
	} else if (buffer == NULL) {
		ms->ms_buffer = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_buffer_size = buffer_size;
	if (filesize != NULL && sgx_is_within_enclave(filesize, _len_filesize)) {
		ms->ms_filesize = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_filesize);
		memset(ms->ms_filesize, 0, _len_filesize);
	} else if (filesize == NULL) {
		ms->ms_filesize = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(1, ms);

	if (buffer) memcpy((void*)buffer, ms->ms_buffer, _len_buffer);
	if (filesize) memcpy((void*)filesize, ms->ms_filesize, _len_filesize);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_get_time(int* second, int* nanosecond)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_second = 4;
	size_t _len_nanosecond = 4;

	ms_ocall_get_time_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_time_t);
	void *__tmp = NULL;

	ocalloc_size += (second != NULL && sgx_is_within_enclave(second, _len_second)) ? _len_second : 0;
	ocalloc_size += (nanosecond != NULL && sgx_is_within_enclave(nanosecond, _len_nanosecond)) ? _len_nanosecond : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_time_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_time_t));

	if (second != NULL && sgx_is_within_enclave(second, _len_second)) {
		ms->ms_second = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_second);
		memset(ms->ms_second, 0, _len_second);
	} else if (second == NULL) {
		ms->ms_second = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (nanosecond != NULL && sgx_is_within_enclave(nanosecond, _len_nanosecond)) {
		ms->ms_nanosecond = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_nanosecond);
		memset(ms->ms_nanosecond, 0, _len_nanosecond);
	} else if (nanosecond == NULL) {
		ms->ms_nanosecond = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(2, ms);

	if (second) memcpy((void*)second, ms->ms_second, _len_second);
	if (nanosecond) memcpy((void*)nanosecond, ms->ms_nanosecond, _len_nanosecond);

	sgx_ocfree();
	return status;
}

