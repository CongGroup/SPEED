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


typedef struct ms_ecall_entrance_t {
	int ms_id;
	char* ms_path;
	int ms_count;
	int ms_dedup_switch;
} ms_ecall_entrance_t;

typedef struct ms_ecall_map_thread_t {
	int ms_no;
} ms_ecall_map_thread_t;

typedef struct ms_ecall_reduce_thread_t {
	int ms_no;
} ms_ecall_reduce_thread_t;

typedef struct ms_ocall_print_string_t {
	char* ms_str;
} ms_ocall_print_string_t;

typedef struct ms_ocall_load_text_file_t {
	char* ms_filename;
	char** ms_buffer;
	int ms_buffer_size;
	int* ms_filesize;
} ms_ocall_load_text_file_t;

typedef struct ms_ocall_write_text_file_t {
	char* ms_filename;
	char* ms_buffer;
	int ms_buffer_size;
} ms_ocall_write_text_file_t;

typedef struct ms_ocall_get_time_t {
	int* ms_second;
	int* ms_nanosecond;
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

typedef struct ms_ocall_begin_map_t {
	int ms_mapper_count;
} ms_ocall_begin_map_t;

typedef struct ms_ocall_begin_reduce_t {
	int ms_reducer_count;
} ms_ocall_begin_reduce_t;

typedef struct ms_ocall_open_t {
	int ms_retval;
	char* ms_filename;
	int ms_mode;
} ms_ocall_open_t;

typedef struct ms_ocall_read_t {
	int ms_retval;
	int ms_file;
	void* ms_buf;
	unsigned int ms_size;
} ms_ocall_read_t;

typedef struct ms_ocall_write_t {
	int ms_retval;
	int ms_file;
	void* ms_buf;
	unsigned int ms_size;
} ms_ocall_write_t;

typedef struct ms_ocall_close_t {
	int ms_file;
} ms_ocall_close_t;

typedef struct ms_ocall_file_size_t {
	int ms_retval;
	char* ms_filename;
} ms_ocall_file_size_t;

typedef struct ms_ocall_read_dir_t {
	int ms_retval;
	char* ms_filename;
	char* ms_buffer;
	int ms_max_file_count;
	unsigned int ms_size;
} ms_ocall_read_dir_t;

typedef struct ms_ocall_delete_array_t {
	char* ms_pointer;
} ms_ocall_delete_array_t;

typedef struct ms_ocall_load_pkt_file_t {
	char* ms_filename;
	char** ms_buffer;
	int* ms_pkt_count;
} ms_ocall_load_pkt_file_t;

typedef struct ms_ocall_get_network_get_time_t {
	int ms_retval;
} ms_ocall_get_network_get_time_t;

typedef struct ms_ocall_get_network_put_time_t {
	int ms_retval;
} ms_ocall_get_network_put_time_t;

typedef struct ms_ocall_free_t {
	void* ms_pointer;
	int ms_isArray;
} ms_ocall_free_t;

typedef struct ms_sgx_thread_wait_untrusted_event_ocall_t {
	int ms_retval;
	void* ms_self;
} ms_sgx_thread_wait_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_set_untrusted_event_ocall_t {
	int ms_retval;
	void* ms_waiter;
} ms_sgx_thread_set_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_setwait_untrusted_events_ocall_t {
	int ms_retval;
	void* ms_waiter;
	void* ms_self;
} ms_sgx_thread_setwait_untrusted_events_ocall_t;

typedef struct ms_sgx_thread_set_multiple_untrusted_events_ocall_t {
	int ms_retval;
	void** ms_waiters;
	size_t ms_total;
} ms_sgx_thread_set_multiple_untrusted_events_ocall_t;

static sgx_status_t SGX_CDECL sgx_ecall_entrance(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_entrance_t));
	ms_ecall_entrance_t* ms = SGX_CAST(ms_ecall_entrance_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_path = ms->ms_path;
	size_t _len_path = _tmp_path ? strlen(_tmp_path) + 1 : 0;
	char* _in_path = NULL;

	CHECK_UNIQUE_POINTER(_tmp_path, _len_path);

	if (_tmp_path != NULL && _len_path != 0) {
		_in_path = (char*)malloc(_len_path);
		if (_in_path == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy((void*)_in_path, _tmp_path, _len_path);
		_in_path[_len_path - 1] = '\0';
	}
	ecall_entrance(ms->ms_id, (const char*)_in_path, ms->ms_count, ms->ms_dedup_switch);
err:
	if (_in_path) free((void*)_in_path);

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_map_thread(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_map_thread_t));
	ms_ecall_map_thread_t* ms = SGX_CAST(ms_ecall_map_thread_t*, pms);
	sgx_status_t status = SGX_SUCCESS;


	ecall_map_thread(ms->ms_no);


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_reduce_thread(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_ecall_reduce_thread_t));
	ms_ecall_reduce_thread_t* ms = SGX_CAST(ms_ecall_reduce_thread_t*, pms);
	sgx_status_t status = SGX_SUCCESS;


	ecall_reduce_thread(ms->ms_no);


	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[3];
} g_ecall_table = {
	3,
	{
		{(void*)(uintptr_t)sgx_ecall_entrance, 0},
		{(void*)(uintptr_t)sgx_ecall_map_thread, 0},
		{(void*)(uintptr_t)sgx_ecall_reduce_thread, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[24][3];
} g_dyn_entry_table = {
	24,
	{
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
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

sgx_status_t SGX_CDECL ocall_load_text_file(const char* filename, char** buffer, int buffer_size, int* filesize)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;
	size_t _len_buffer = sizeof(*buffer);
	size_t _len_filesize = sizeof(*filesize);

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
		ms->ms_buffer = (char**)__tmp;
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

sgx_status_t SGX_CDECL ocall_write_text_file(const char* filename, const char* buffer, int buffer_size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;
	size_t _len_buffer = buffer_size;

	ms_ocall_write_text_file_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_write_text_file_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;
	ocalloc_size += (buffer != NULL && sgx_is_within_enclave(buffer, _len_buffer)) ? _len_buffer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_write_text_file_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_write_text_file_t));

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
		memcpy((void*)ms->ms_buffer, buffer, _len_buffer);
	} else if (buffer == NULL) {
		ms->ms_buffer = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_buffer_size = buffer_size;
	status = sgx_ocall(2, ms);


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
	
	status = sgx_ocall(3, ms);

	if (second) memcpy((void*)second, ms->ms_second, _len_second);
	if (nanosecond) memcpy((void*)nanosecond, ms->ms_nanosecond, _len_nanosecond);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_request_find(const uint8_t* tag, uint8_t* meta, uint8_t* rlt, int expt_size, int* true_size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_tag = 32;
	size_t _len_meta = 64;
	size_t _len_rlt = expt_size;
	size_t _len_true_size = sizeof(*true_size);

	ms_ocall_request_find_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_request_find_t);
	void *__tmp = NULL;

	ocalloc_size += (tag != NULL && sgx_is_within_enclave(tag, _len_tag)) ? _len_tag : 0;
	ocalloc_size += (meta != NULL && sgx_is_within_enclave(meta, _len_meta)) ? _len_meta : 0;
	ocalloc_size += (rlt != NULL && sgx_is_within_enclave(rlt, _len_rlt)) ? _len_rlt : 0;
	ocalloc_size += (true_size != NULL && sgx_is_within_enclave(true_size, _len_true_size)) ? _len_true_size : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_request_find_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_request_find_t));

	if (tag != NULL && sgx_is_within_enclave(tag, _len_tag)) {
		ms->ms_tag = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_tag);
		memcpy((void*)ms->ms_tag, tag, _len_tag);
	} else if (tag == NULL) {
		ms->ms_tag = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (meta != NULL && sgx_is_within_enclave(meta, _len_meta)) {
		ms->ms_meta = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_meta);
		memset(ms->ms_meta, 0, _len_meta);
	} else if (meta == NULL) {
		ms->ms_meta = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (rlt != NULL && sgx_is_within_enclave(rlt, _len_rlt)) {
		ms->ms_rlt = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_rlt);
		memset(ms->ms_rlt, 0, _len_rlt);
	} else if (rlt == NULL) {
		ms->ms_rlt = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_expt_size = expt_size;
	if (true_size != NULL && sgx_is_within_enclave(true_size, _len_true_size)) {
		ms->ms_true_size = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_true_size);
		memset(ms->ms_true_size, 0, _len_true_size);
	} else if (true_size == NULL) {
		ms->ms_true_size = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(4, ms);

	if (meta) memcpy((void*)meta, ms->ms_meta, _len_meta);
	if (rlt) memcpy((void*)rlt, ms->ms_rlt, _len_rlt);
	if (true_size) memcpy((void*)true_size, ms->ms_true_size, _len_true_size);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_request_put(const uint8_t* tag, const uint8_t* meta, const uint8_t* rlt, int rlt_size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_tag = 32;
	size_t _len_meta = 64;
	size_t _len_rlt = rlt_size;

	ms_ocall_request_put_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_request_put_t);
	void *__tmp = NULL;

	ocalloc_size += (tag != NULL && sgx_is_within_enclave(tag, _len_tag)) ? _len_tag : 0;
	ocalloc_size += (meta != NULL && sgx_is_within_enclave(meta, _len_meta)) ? _len_meta : 0;
	ocalloc_size += (rlt != NULL && sgx_is_within_enclave(rlt, _len_rlt)) ? _len_rlt : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_request_put_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_request_put_t));

	if (tag != NULL && sgx_is_within_enclave(tag, _len_tag)) {
		ms->ms_tag = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_tag);
		memcpy((void*)ms->ms_tag, tag, _len_tag);
	} else if (tag == NULL) {
		ms->ms_tag = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (meta != NULL && sgx_is_within_enclave(meta, _len_meta)) {
		ms->ms_meta = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_meta);
		memcpy((void*)ms->ms_meta, meta, _len_meta);
	} else if (meta == NULL) {
		ms->ms_meta = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (rlt != NULL && sgx_is_within_enclave(rlt, _len_rlt)) {
		ms->ms_rlt = (uint8_t*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_rlt);
		memcpy((void*)ms->ms_rlt, rlt, _len_rlt);
	} else if (rlt == NULL) {
		ms->ms_rlt = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_rlt_size = rlt_size;
	status = sgx_ocall(5, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_begin_map(int mapper_count)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_begin_map_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_begin_map_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_begin_map_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_begin_map_t));

	ms->ms_mapper_count = mapper_count;
	status = sgx_ocall(6, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_begin_reduce(int reducer_count)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_begin_reduce_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_begin_reduce_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_begin_reduce_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_begin_reduce_t));

	ms->ms_reducer_count = reducer_count;
	status = sgx_ocall(7, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_open(int* retval, const char* filename, int mode)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;

	ms_ocall_open_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_open_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_open_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_open_t));

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
	
	ms->ms_mode = mode;
	status = sgx_ocall(8, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_read(int* retval, int file, void* buf, unsigned int size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_buf = size;

	ms_ocall_read_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_read_t);
	void *__tmp = NULL;

	ocalloc_size += (buf != NULL && sgx_is_within_enclave(buf, _len_buf)) ? _len_buf : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_read_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_read_t));

	ms->ms_file = file;
	if (buf != NULL && sgx_is_within_enclave(buf, _len_buf)) {
		ms->ms_buf = (void*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_buf);
		memset(ms->ms_buf, 0, _len_buf);
	} else if (buf == NULL) {
		ms->ms_buf = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_size = size;
	status = sgx_ocall(9, ms);

	if (retval) *retval = ms->ms_retval;
	if (buf) memcpy((void*)buf, ms->ms_buf, _len_buf);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_write(int* retval, int file, void* buf, unsigned int size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_buf = size;

	ms_ocall_write_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_write_t);
	void *__tmp = NULL;

	ocalloc_size += (buf != NULL && sgx_is_within_enclave(buf, _len_buf)) ? _len_buf : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_write_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_write_t));

	ms->ms_file = file;
	if (buf != NULL && sgx_is_within_enclave(buf, _len_buf)) {
		ms->ms_buf = (void*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_buf);
		memcpy(ms->ms_buf, buf, _len_buf);
	} else if (buf == NULL) {
		ms->ms_buf = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_size = size;
	status = sgx_ocall(10, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_close(int file)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_close_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_close_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_close_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_close_t));

	ms->ms_file = file;
	status = sgx_ocall(11, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_file_size(int* retval, const char* filename)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;

	ms_ocall_file_size_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_file_size_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_file_size_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_file_size_t));

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
	
	status = sgx_ocall(12, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_read_dir(int* retval, const char* filename, char* buffer, int max_file_count, unsigned int size)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;
	size_t _len_buffer = size;

	ms_ocall_read_dir_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_read_dir_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;
	ocalloc_size += (buffer != NULL && sgx_is_within_enclave(buffer, _len_buffer)) ? _len_buffer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_read_dir_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_read_dir_t));

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
	
	ms->ms_max_file_count = max_file_count;
	ms->ms_size = size;
	status = sgx_ocall(13, ms);

	if (retval) *retval = ms->ms_retval;
	if (buffer) memcpy((void*)buffer, ms->ms_buffer, _len_buffer);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_delete_array(char* pointer)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_pointer = sizeof(*pointer);

	ms_ocall_delete_array_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_delete_array_t);
	void *__tmp = NULL;

	ocalloc_size += (pointer != NULL && sgx_is_within_enclave(pointer, _len_pointer)) ? _len_pointer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_delete_array_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_delete_array_t));

	if (pointer != NULL && sgx_is_within_enclave(pointer, _len_pointer)) {
		ms->ms_pointer = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_pointer);
		memcpy(ms->ms_pointer, pointer, _len_pointer);
	} else if (pointer == NULL) {
		ms->ms_pointer = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(14, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_load_pkt_file(const char* filename, char** buffer, int* pkt_count)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_filename = filename ? strlen(filename) + 1 : 0;
	size_t _len_buffer = sizeof(*buffer);
	size_t _len_pkt_count = sizeof(*pkt_count);

	ms_ocall_load_pkt_file_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_load_pkt_file_t);
	void *__tmp = NULL;

	ocalloc_size += (filename != NULL && sgx_is_within_enclave(filename, _len_filename)) ? _len_filename : 0;
	ocalloc_size += (buffer != NULL && sgx_is_within_enclave(buffer, _len_buffer)) ? _len_buffer : 0;
	ocalloc_size += (pkt_count != NULL && sgx_is_within_enclave(pkt_count, _len_pkt_count)) ? _len_pkt_count : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_load_pkt_file_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_load_pkt_file_t));

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
		ms->ms_buffer = (char**)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_buffer);
		memset(ms->ms_buffer, 0, _len_buffer);
	} else if (buffer == NULL) {
		ms->ms_buffer = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	if (pkt_count != NULL && sgx_is_within_enclave(pkt_count, _len_pkt_count)) {
		ms->ms_pkt_count = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_pkt_count);
		memcpy(ms->ms_pkt_count, pkt_count, _len_pkt_count);
	} else if (pkt_count == NULL) {
		ms->ms_pkt_count = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(15, ms);

	if (buffer) memcpy((void*)buffer, ms->ms_buffer, _len_buffer);
	if (pkt_count) memcpy((void*)pkt_count, ms->ms_pkt_count, _len_pkt_count);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_get_network_get_time(int* retval)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_get_network_get_time_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_network_get_time_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_network_get_time_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_network_get_time_t));

	status = sgx_ocall(16, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_get_network_put_time(int* retval)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_get_network_put_time_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_network_put_time_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_network_put_time_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_network_put_time_t));

	status = sgx_ocall(17, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_free(void* pointer, int isArray)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_free_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_free_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_free_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_free_t));

	ms->ms_pointer = SGX_CAST(void*, pointer);
	ms->ms_isArray = isArray;
	status = sgx_ocall(18, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_put_get_time()
{
	sgx_status_t status = SGX_SUCCESS;
	status = sgx_ocall(19, NULL);

	return status;
}

sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_wait_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t));

	ms->ms_self = SGX_CAST(void*, self);
	status = sgx_ocall(20, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_set_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_untrusted_event_ocall_t));

	ms->ms_waiter = SGX_CAST(void*, waiter);
	status = sgx_ocall(21, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_setwait_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t));

	ms->ms_waiter = SGX_CAST(void*, waiter);
	ms->ms_self = SGX_CAST(void*, self);
	status = sgx_ocall(22, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_waiters = total * sizeof(*waiters);

	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t);
	void *__tmp = NULL;

	ocalloc_size += (waiters != NULL && sgx_is_within_enclave(waiters, _len_waiters)) ? _len_waiters : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_multiple_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t));

	if (waiters != NULL && sgx_is_within_enclave(waiters, _len_waiters)) {
		ms->ms_waiters = (void**)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_waiters);
		memcpy((void*)ms->ms_waiters, waiters, _len_waiters);
	} else if (waiters == NULL) {
		ms->ms_waiters = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_total = total;
	status = sgx_ocall(23, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

