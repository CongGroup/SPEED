#ifndef CRYPTO_H
#define CRYPTO_H

#include "sgx_tcrypto.h"

#include "../common/data_type.h"

void key_gen(byte *key, int len);

int auth_enc(byte *key, int key_len,
             byte *src, int src_len, 
             byte *dst, byte *out_mac);

int veri_dec(byte *key, int key_len,
             byte *src, int src_len, 
             byte *dst, const byte *in_mac);

void draw_rand(byte *r, int len);

void hash(const byte *src, int src_len, byte *hash);

#endif