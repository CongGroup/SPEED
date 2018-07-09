#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


void ecall_entrance(int id, const char* path, int count);
void ecall_map_thread(int no);
void ecall_reduce_thread(int no);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_load_text_file(const char* filename, char** buffer, int buffer_size, int* filesize);
sgx_status_t SGX_CDECL ocall_write_text_file(const char* filename, const char* buffer, int buffer_size);
sgx_status_t SGX_CDECL ocall_get_time(int* second, int* nanosecond);
sgx_status_t SGX_CDECL ocall_request_find(const uint8_t* tag, uint8_t* meta, uint8_t* rlt, int expt_size, int* true_size);
sgx_status_t SGX_CDECL ocall_request_put(const uint8_t* tag, const uint8_t* meta, const uint8_t* rlt, int rlt_size);
sgx_status_t SGX_CDECL ocall_begin_map(int mapper_count);
sgx_status_t SGX_CDECL ocall_begin_reduce(int reducer_count);
sgx_status_t SGX_CDECL ocall_open(int* retval, const char* filename, int mode);
sgx_status_t SGX_CDECL ocall_read(int* retval, int file, void* buf, unsigned int size);
sgx_status_t SGX_CDECL ocall_write(int* retval, int file, void* buf, unsigned int size);
sgx_status_t SGX_CDECL ocall_close(int file);
sgx_status_t SGX_CDECL ocall_file_size(int* retval, const char* filename);
sgx_status_t SGX_CDECL ocall_read_dir(int* retval, const char* filename, char* buffer, int max_file_count, unsigned int size);
sgx_status_t SGX_CDECL ocall_delete_array(char* pointer);
sgx_status_t SGX_CDECL ocall_load_pkt_file(const char* filename, char** buffer, int* pkt_count);
sgx_status_t SGX_CDECL ocall_get_network_get_time(int* retval);
sgx_status_t SGX_CDECL ocall_get_network_put_time(int* retval);
sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter);
sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
