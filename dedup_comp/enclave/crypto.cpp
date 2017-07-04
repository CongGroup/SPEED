#include "crypto.h"

#include "sysutils.h"

#include "sgx_trts.h"

//const sgx_aes_gcm_128bit_key_t p_key = { 0 };
//sgx_aes_gcm_128bit_tag_t p_out_mac = { 0 };
#define NIST_IV_LEN 12
const byte p_iv[NIST_IV_LEN] = { 0 };

void key_gen(byte *key, int len) {
    draw_rand(key, len);
}

// AES-GCM-128
int auth_enc(byte *key, int key_len,
             byte * src, int src_len, 
             byte *dst, byte *out_mac) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_rijndael128GCM_encrypt(
        (sgx_aes_gcm_128bit_key_t*)key,
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

// AES-GCM-128
int veri_dec(byte *key, int key_len,
             byte *src, int src_len, 
             byte *dst, const byte *in_mac) {
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_rijndael128GCM_decrypt(
        (sgx_aes_gcm_128bit_key_t*)key,
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

void draw_rand(byte *r, int len)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_read_rand(r, len);
}

void hash(const byte *src, int src_len, byte * hash)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    ret = sgx_sha256_msg(
        src,
        src_len,
        (sgx_sha256_hash_t *)hash);
}
