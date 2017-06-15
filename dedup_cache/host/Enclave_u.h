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

sgx_status_t ecall_cache_get(sgx_enclave_id_t eid, int* retval, char* tag, char* rlt, int rlt_size, char* r);
sgx_status_t ecall_cache_put(sgx_enclave_id_t eid, char* tag, char* rlt, int rlt_size, char* r);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
