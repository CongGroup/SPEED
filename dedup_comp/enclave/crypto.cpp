#include "crypto.h"

#include "sysutils.h"

const sgx_aes_gcm_128bit_key_t p_key = { 0 };
sgx_aes_gcm_128bit_tag_t p_out_mac = { 0 };
#define NIST_IV_LEN 12
const uint8_t p_iv[NIST_IV_LEN] = { 0 };

int auth_enc(uint8_t * src, int src_len, uint8_t *dst, uint8_t *out_mac) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_rijndael128GCM_encrypt(
        &p_key,
        src, src_len,
        dst,
        p_iv, NIST_IV_LEN,
        0, 0, // no additional authentication data
        (sgx_aes_gcm_128bit_tag_t*)out_mac);
    if (ret == SGX_SUCCESS) {
        return 1;
    }
    else {
        //eprintf("[*] auth_enc error: %d\n", ret);
        return 0;
    }
}

int veri_dec(uint8_t *src, int src_len, uint8_t *dst, const uint8_t *in_mac) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_rijndael128GCM_decrypt(
        &p_key,
        src, src_len,
        dst,
        p_iv, NIST_IV_LEN,
        0, 0, // no additional authentication data
        (sgx_aes_gcm_128bit_tag_t*)in_mac);
    if (ret == SGX_SUCCESS) {
        return 1;
    }
    else {
        //eprintf("[*] veri_dec error: %d\n", ret);
        return 0;
    }
}

void draw_rand(uint8_t * r, int length)
{
    // TODO
}

void hash(const uint8_t * src, int src_len, uint8_t * hash)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_sha256_msg(
        src,
        src_len,
        (sgx_sha256_hash_t *)hash);
}
