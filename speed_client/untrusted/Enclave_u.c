#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_ecall_entrance_t {
	int ms_id;
	char* ms_path;
	size_t ms_path_len;
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
	char** ms_buffer;
	int ms_file_path_len;
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

static sgx_status_t SGX_CDECL Enclave_ocall_write_text_file(void* pms)
{
	ms_ocall_write_text_file_t* ms = SGX_CAST(ms_ocall_write_text_file_t*, pms);
	ocall_write_text_file((const char*)ms->ms_filename, (const char*)ms->ms_buffer, ms->ms_buffer_size);

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

static sgx_status_t SGX_CDECL Enclave_ocall_begin_map(void* pms)
{
	ms_ocall_begin_map_t* ms = SGX_CAST(ms_ocall_begin_map_t*, pms);
	ocall_begin_map(ms->ms_mapper_count);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_begin_reduce(void* pms)
{
	ms_ocall_begin_reduce_t* ms = SGX_CAST(ms_ocall_begin_reduce_t*, pms);
	ocall_begin_reduce(ms->ms_reducer_count);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_open(void* pms)
{
	ms_ocall_open_t* ms = SGX_CAST(ms_ocall_open_t*, pms);
	ms->ms_retval = ocall_open((const char*)ms->ms_filename, ms->ms_mode);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_read(void* pms)
{
	ms_ocall_read_t* ms = SGX_CAST(ms_ocall_read_t*, pms);
	ms->ms_retval = ocall_read(ms->ms_file, ms->ms_buf, ms->ms_size);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_write(void* pms)
{
	ms_ocall_write_t* ms = SGX_CAST(ms_ocall_write_t*, pms);
	ms->ms_retval = ocall_write(ms->ms_file, ms->ms_buf, ms->ms_size);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_close(void* pms)
{
	ms_ocall_close_t* ms = SGX_CAST(ms_ocall_close_t*, pms);
	ocall_close(ms->ms_file);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_file_size(void* pms)
{
	ms_ocall_file_size_t* ms = SGX_CAST(ms_ocall_file_size_t*, pms);
	ms->ms_retval = ocall_file_size((const char*)ms->ms_filename);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_read_dir(void* pms)
{
	ms_ocall_read_dir_t* ms = SGX_CAST(ms_ocall_read_dir_t*, pms);
	ms->ms_retval = ocall_read_dir((const char*)ms->ms_filename, ms->ms_buffer, ms->ms_file_path_len);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_delete_array(void* pms)
{
	ms_ocall_delete_array_t* ms = SGX_CAST(ms_ocall_delete_array_t*, pms);
	ocall_delete_array(ms->ms_pointer);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_load_pkt_file(void* pms)
{
	ms_ocall_load_pkt_file_t* ms = SGX_CAST(ms_ocall_load_pkt_file_t*, pms);
	ocall_load_pkt_file((const char*)ms->ms_filename, ms->ms_buffer, ms->ms_pkt_count);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_get_network_get_time(void* pms)
{
	ms_ocall_get_network_get_time_t* ms = SGX_CAST(ms_ocall_get_network_get_time_t*, pms);
	ms->ms_retval = ocall_get_network_get_time();

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_get_network_put_time(void* pms)
{
	ms_ocall_get_network_put_time_t* ms = SGX_CAST(ms_ocall_get_network_put_time_t*, pms);
	ms->ms_retval = ocall_get_network_put_time();

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_free(void* pms)
{
	ms_ocall_free_t* ms = SGX_CAST(ms_ocall_free_t*, pms);
	ocall_free(ms->ms_pointer, ms->ms_isArray);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_put_get_time(void* pms)
{
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ocall_put_get_time();
	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_sgx_thread_wait_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_wait_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_wait_untrusted_event_ocall((const void*)ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_sgx_thread_set_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_set_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_untrusted_event_ocall((const void*)ms->ms_waiter);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_sgx_thread_setwait_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_setwait_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_setwait_untrusted_events_ocall((const void*)ms->ms_waiter, (const void*)ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_sgx_thread_set_multiple_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_multiple_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_multiple_untrusted_events_ocall((const void**)ms->ms_waiters, ms->ms_total);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[24];
} ocall_table_Enclave = {
	24,
	{
		(void*)Enclave_ocall_print_string,
		(void*)Enclave_ocall_load_text_file,
		(void*)Enclave_ocall_write_text_file,
		(void*)Enclave_ocall_get_time,
		(void*)Enclave_ocall_request_find,
		(void*)Enclave_ocall_request_put,
		(void*)Enclave_ocall_begin_map,
		(void*)Enclave_ocall_begin_reduce,
		(void*)Enclave_ocall_open,
		(void*)Enclave_ocall_read,
		(void*)Enclave_ocall_write,
		(void*)Enclave_ocall_close,
		(void*)Enclave_ocall_file_size,
		(void*)Enclave_ocall_read_dir,
		(void*)Enclave_ocall_delete_array,
		(void*)Enclave_ocall_load_pkt_file,
		(void*)Enclave_ocall_get_network_get_time,
		(void*)Enclave_ocall_get_network_put_time,
		(void*)Enclave_ocall_free,
		(void*)Enclave_ocall_put_get_time,
		(void*)Enclave_sgx_thread_wait_untrusted_event_ocall,
		(void*)Enclave_sgx_thread_set_untrusted_event_ocall,
		(void*)Enclave_sgx_thread_setwait_untrusted_events_ocall,
		(void*)Enclave_sgx_thread_set_multiple_untrusted_events_ocall,
	}
};
sgx_status_t ecall_entrance(sgx_enclave_id_t eid, int id, const char* path, int count, int dedup_switch)
{
	sgx_status_t status;
	ms_ecall_entrance_t ms;
	ms.ms_id = id;
	ms.ms_path = (char*)path;
	ms.ms_path_len = path ? strlen(path) + 1 : 0;
	ms.ms_count = count;
	ms.ms_dedup_switch = dedup_switch;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_map_thread(sgx_enclave_id_t eid, int no)
{
	sgx_status_t status;
	ms_ecall_map_thread_t ms;
	ms.ms_no = no;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t ecall_reduce_thread(sgx_enclave_id_t eid, int no)
{
	sgx_status_t status;
	ms_ecall_reduce_thread_t ms;
	ms.ms_no = no;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	return status;
}

