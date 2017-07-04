#include "dedup_service.h"

#include "crypto.h"
#include "sysutils.h"

#include "Enclave_t.h"

#include "../../common/config.h"
#include "../../common/data_type.h"

#include <assert.h>
#include <cstring>

#define RAW(data) ((byte*)(data))

// VERY specific to the scheme, not intended for any other usage
inline byte *rand_hash(const byte *r, Function *func) {
    static byte h[HASH_SIZE];
    
    int type_id = func->get_type();
    int h_in_size = RAND_SIZE + sizeof(int) + func->input_size();
    byte *h_in = new byte[h_in_size];
    memcpy(h_in, r, RAND_SIZE);
    memcpy(h_in + RAND_SIZE, &type_id, sizeof(int));
    memcpy(h_in + RAND_SIZE + sizeof(int), func->input(), func->input_size());
    
    hash(h_in, h_in_size, h);
    delete h_in;

    return h;
}

// VERY specific to the scheme, not intended for any other usage
inline byte *rand_key() {
    static byte k[ENC_KEY_SIZE];
    key_gen(k, ENC_KEY_SIZE);

    return k;
}

inline void byte_or(const byte *ina, const byte *inb,
                    int len, byte *out) {
    for (int i = 0; i < len; ++i)
        out[i] = ina[i] ^ inb[i];
}

void dedup(Function *func)
{
    eprintf("[*] < %s - %d > ", func->get_name(), func->get_id());

    int true_size = 0;
    metadata meta;

    ocall_request_find(func->get_tag(),
                       RAW(&meta),
                       func->output(), func->expt_output_size(),
                       &true_size);

    // hit
    if (true_size > 0) {
        assert(true_size <= func->expt_output_size());

        eprintf("--> fetched ");

        byte *h = rand_hash(meta.r, func);

        byte k[ENC_KEY_SIZE];
        
        byte_or(meta.enc_key, h, ENC_KEY_SIZE, k);

        if (veri_dec(k, ENC_KEY_SIZE,
                     func->output(), true_size, 
                     func->output(), meta.mac)) {
            eprintf("--> verified and decrypted!\n");
        }
        else {
            eprintf("--> unverified, the data may have been corrupted at the caching server!\n");
        }
    }
    // miss
    else {
        eprintf("--> missed ");
        
        func->process();

        draw_rand(meta.r, RAND_SIZE);

        byte *h = rand_hash(meta.r, func);

        byte *k = rand_key();

        auth_enc(k, ENC_KEY_SIZE,
                 func->output(), func->expt_output_size(), 
                 func->output(), meta.mac);

        byte_or(k, h, ENC_KEY_SIZE, meta.enc_key);

        eprintf("--> locally processed ");

        ocall_request_put(func->get_tag(),
                          RAW(&meta),
                          func->output(), func->expt_output_size());
        
        // TODO check put response
        eprintf("--> remotely cached!\n");
    }
}