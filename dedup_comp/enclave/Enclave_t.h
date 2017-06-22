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


void ecall_entrance();

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_load_text_file(const char* filename, char* buffer, int buffer_size, int* filesize);
sgx_status_t SGX_CDECL ocall_get_time(long int* second, long int* nanosecond);
sgx_status_t SGX_CDECL ocall_request_find(const uint8_t* tag, int* resp_size, uint8_t* rlt, int exp_size, uint8_t* meta);
sgx_status_t SGX_CDECL ocall_request_put(const uint8_t* tag, const uint8_t* rlt, int rlt_size, const uint8_t* meta);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
