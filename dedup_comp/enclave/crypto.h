#ifndef CRYPTO_H
#define CRYPTO_H

#include "sgx_tcrypto.h"

int auth_enc(uint8_t *src, int src_len, uint8_t *dst, uint8_t *out_mac);

int veri_dec(uint8_t *src, int src_len, uint8_t *dst, const uint8_t *in_mac);

void draw_rand(uint8_t *r, int length);

void hash(const uint8_t *src, int src_len, uint8_t *hash);

#endif