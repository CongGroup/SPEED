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


void ecall_test();

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_get_time(int* second, int* nanosecond);
sgx_status_t SGX_CDECL ocall_get_data(int data_id, char* val, size_t* len);
sgx_status_t SGX_CDECL ocall_open(int* retval, const char* filename, int mode);
sgx_status_t SGX_CDECL ocall_read(int* retval, int file, void* buf, unsigned int size);
sgx_status_t SGX_CDECL ocall_write(int* retval, int file, void* buf, unsigned int size);
sgx_status_t SGX_CDECL ocall_close(int file);
sgx_status_t SGX_CDECL ocall_rand(int* rand_num, int mod);
sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter);
sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
