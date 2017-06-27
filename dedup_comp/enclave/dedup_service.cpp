#include "dedup_service.h"

#include "crypto.h"
#include "sysutils.h"

#include "Enclave_t.h"

#include "../../common/config.h"
#include "../../common/data_type.h"

#include <assert.h>

void dedup(Function *func)
{
    eprintf("[*] [ %s - %d ] ", func->get_name(), func->get_id());

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

        // decrypt
        if (veri_dec(func->output(), true_size, func->output(), meta.mac)) {
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

        auth_enc(func->output(), func->expt_output_size(), func->output(), meta.mac);

        eprintf("--> locally processed ");

        draw_rand(meta.r, RAND_SIZE);

        ocall_request_put(func->get_tag(),
                          RAW(&meta),
                          func->output(), func->expt_output_size());
        
        // TODO check put response
        eprintf("--> remotely cached!\n");
    }
}