#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_load_text_file, (const char* filename, char** buffer, int buffer_size, int* filesize));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_write_text_file, (const char* filename, const char* buffer, int buffer_size));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_time, (int* second, int* nanosecond));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_request_find, (const uint8_t* tag, uint8_t* meta, uint8_t* rlt, int expt_size, int* true_size));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_request_put, (const uint8_t* tag, const uint8_t* meta, const uint8_t* rlt, int rlt_size));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_begin_map, (int mapper_count));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_begin_reduce, (int reducer_count));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_open, (const char* filename, int mode));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_read, (int file, void* buf, unsigned int size));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_write, (int file, void* buf, unsigned int size));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_close, (int file));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_file_size, (const char* filename));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_read_dir, (const char* filename, char* buffer, int max_file_count, unsigned int size));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_delete_array, (char* pointer));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_load_pkt_file, (const char* filename, char** buffer, int* pkt_count));
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_network_get_time, ());
int SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_network_put_time, ());
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_free, (void* pointer, int isArray));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_put_get_time, ());
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_wait_untrusted_event_ocall, (const void* self));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_set_untrusted_event_ocall, (const void* waiter));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_setwait_untrusted_events_ocall, (const void* waiter, const void* self));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_set_multiple_untrusted_events_ocall, (const void** waiters, size_t total));

sgx_status_t ecall_entrance(sgx_enclave_id_t eid, int id, const char* path, int count, int dedup_switch);
sgx_status_t ecall_map_thread(sgx_enclave_id_t eid, int no);
sgx_status_t ecall_reduce_thread(sgx_enclave_id_t eid, int no);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
