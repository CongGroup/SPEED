#include "dedup_service.h"

#include "crypto.h"
#include "sysutils.h"

#include "Enclave_t.h"

#include "../../common/config.h"
#include "../../common/data_type.h"

#include <assert.h>

void dedup(Function *func)
{
    eprintf("[*] Dedup service launched!\n");

    int true_size = 0;
    metadata meta;

    ocall_request_find(
        func->get_tag(),
        RAW(&meta),
        func->output(), func->expt_output_size(),
        &true_size);

    // hit
    if (true_size > 0) {
        assert(true_size <= func->expt_output_size());

        eprintf("[*] %s with id <%d> successfuly fetched!\n", func->get_name(), func->get_id());

        // decrypt
        if (veri_dec(func->output(), true_size, func->output(), meta.mac)) {
            eprintf("    The result is verified and decrypted!\n");
        }
        else {
            eprintf("    The result cannot be verified, the data may have been corrupted at the cache server!\n");
        }
    }
    // miss
    else {
        eprintf("[*] %s with id <%d> cannot be found at the server! Now process locally ...\n", func->get_name(), func->get_id());
        
        func->process();

        auth_enc(func->output(), func->expt_output_size(), func->output(), meta.mac);

        draw_rand(meta.r, RAND_SIZE);

        ocall_request_put(
            func->get_tag(),
            RAW(&meta),
            func->output(), func->expt_output_size());
        
        eprintf("    The computation result is cached!\n");
    }
}