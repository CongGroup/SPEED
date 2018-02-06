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
    
	//int type_id = func->get_type();
	//int h_in_size = RAND_SIZE + sizeof(int) + func->input_size();
    //byte *h_in = new byte[h_in_size];
    //memcpy(h_in, r, RAND_SIZE);
    //memcpy(h_in + RAND_SIZE, &type_id, sizeof(int));
    //memcpy(h_in + RAND_SIZE + sizeof(int), func->input(), func->input_size());
    //
    //hash(h_in, h_in_size, h);
    //delete h_in;

	//eprintf("&&&DEBUG RAND_HASH input size is %d \n", func->input_size());

	hash(func->input(), func->input_size(), h);

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

void printBlock(const void* src, int size)
{
	for (int i = 0; i < size; ++i)
	{
		eprintf("%.2X ", *((const unsigned char*)src + i));
		if (i % 16 == 15 || i == size - 1)
			eprintf("\n");
	}
}

void dedup(Function *func)
{
	hrtime time_tag_1, time_tag_2, time_tag_3,
		time_tag_4, time_tag_5, time_tag_6, time_tag_7,
		time_tag_8, time_tag_9, time_tag_10, time_tag_11, time_tag_12;

    eprintf("[*] < %s - %d > \n", func->get_name(), func->get_id());

    int true_size = 0;
    metadata meta;

	get_time(&time_tag_1);

	const byte* func_tag = func->get_tag();

	get_time(&time_tag_2);

	eprintf("Time for compute tag is [%d us]\n", time_elapsed_in_us(&time_tag_1, &time_tag_2));

	//printBlock(func->get_tag(), TAG_SIZE);

	get_time(&time_tag_2);

	ecall_cache_get(func_tag,
                       RAW(&meta),
                       func->output(), func->expt_output_size(),
                       &true_size);

	get_time(&time_tag_3);

	eprintf("Time for find tag with server is [%d us]\n", time_elapsed_in_us(&time_tag_2, &time_tag_3));

	//printBlock(func->get_tag(), TAG_SIZE);

    // hit
    if (true_size > 0) {
        assert(true_size <= func->expt_output_size());

        eprintf("--> fetched \n");

		byte k[ENC_KEY_SIZE];

		get_time(&time_tag_3);

		func->setR(meta.r);

        byte *h = rand_hash(meta.r, func);
        
        byte_or(meta.enc_key, h, ENC_KEY_SIZE, k);

		get_time(&time_tag_4);

		eprintf("Time for recover key is [%d us]\n", time_elapsed_in_us(&time_tag_3, &time_tag_4));

		get_time(&time_tag_4);

		int decRes = veri_dec(k, ENC_KEY_SIZE,
			func->output(), true_size,
			func->output(), meta.mac);
		get_time(&time_tag_5);
		eprintf("Time for dec res is [%d us], true_size is %d\n", time_elapsed_in_us(&time_tag_4, &time_tag_5), true_size);
		if (decRes) {
			eprintf("--> verified and decrypted!\n");
		}
		else {
			eprintf("--> unverified, the data may have been corrupted at the caching server!\n");
		}
		get_time(&time_tag_6);
		veri_dec(k, ENC_KEY_SIZE,func->input(), func->input_size(), func->input(), meta.mac);
		get_time(&time_tag_7);
		eprintf("Time for dec on File Size is [%d us]\n", time_elapsed_in_us(&time_tag_6, &time_tag_7));

    }
    // miss
    else {
        eprintf("--> missed \n");
        
		get_time(&time_tag_4);

        func->process();

		get_time(&time_tag_5);

		eprintf("Time for process result is [%d us], res size is %d \n", time_elapsed_in_us(&time_tag_4, &time_tag_5), func->expt_output_size());

		get_time(&time_tag_5);

        draw_rand(meta.r, RAND_SIZE);

		byte *k = rand_key();

		get_time(&time_tag_6);

		func->setR(meta.r);

        byte *h = rand_hash(meta.r, func);

		byte_or(k, h, ENC_KEY_SIZE, meta.enc_key);

		get_time(&time_tag_7);

		eprintf("Time for gen key is [%d us]\n", time_elapsed_in_us(&time_tag_5, &time_tag_7));
		eprintf("Time for recover key is [%d us]\n", time_elapsed_in_us(&time_tag_6, &time_tag_7));

		get_time(&time_tag_6);

        auth_enc(k, ENC_KEY_SIZE,
                 func->output(), func->expt_output_size(), 
                 func->output(), meta.mac);

		get_time(&time_tag_7);

		eprintf("Time for enc res is [%d us]\n", time_elapsed_in_us(&time_tag_6, &time_tag_7));

        eprintf("--> locally processed \n");

		get_time(&time_tag_7);

		func_tag = func->get_tag();

		ecall_cache_put(func_tag,
                          RAW(&meta),
                          func->output(), func->expt_output_size());

		//printBlock(func_tag, TAG_SIZE);

		get_time(&time_tag_8);

		eprintf("Time for put to cache is [%d us]\n", time_elapsed_in_us(&time_tag_7, &time_tag_8));
        

		get_time(&time_tag_9);
		auth_enc(k, ENC_KEY_SIZE,func->input(), func->input_size(), func->input(), meta.mac);
		get_time(&time_tag_10);
		eprintf("Time for enc on File Size %d, use [%d us]\n", func->input_size(),time_elapsed_in_us(&time_tag_9, &time_tag_10));


        // TODO check put response
        eprintf("--> remotely cached!\n");
    }
}

cache_t cache;

void ecall_cache_get(const uint8_t *tag,
	uint8_t *meta,
	uint8_t *rlt, int expt_size,
	int *true_size)
{
	hrtime start_time, end_time;

	//printBlock(tag, TAG_SIZE);

	get_time(&start_time);

	binary key(TAG_SIZE, 0);
	memcpy(&key[0], tag, TAG_SIZE);

	cache_t::const_iterator it = cache.find(key);

#ifndef TEST_PUT_AND_GET
	// cache miss
	if (it == cache.end()) {
		*true_size = 0;
	}
	// cache hit
	else {
		const entry_t &entry = it->second;

		// copy meta data
		memcpy(meta, &entry.first[0], entry.first.size());

		// copy computation result
		*true_size = entry.second.size();
		memcpy(rlt, &entry.second[0], std::min(*true_size, expt_size));
	}

#endif

	get_time(&end_time);

#ifndef TEST_PUT_AND_GET
	eprintf("Get cache use %d us \n ", time_elapsed_in_us(&start_time, &end_time));
#endif




	//eprintf("In get_cache the Map size is %d\n", cache.size());

}

void ecall_cache_put(const uint8_t *tag,
	const uint8_t *meta,
	const uint8_t *rlt, int rlt_size)
{

	hrtime start_time, end_time;

	//PUT
	//eprintf("\n In the Put ", time_elapsed_in_us(&start_time, &end_time));
	//printBlock(tag, TAG_SIZE);

	get_time(&start_time);

	binary key(TAG_SIZE, 0);
	memcpy(&key[0], tag, TAG_SIZE);

	binary entry_meta(sizeof(metadata), 0);
	memcpy(&entry_meta[0], meta, sizeof(metadata));

#ifndef TEST_PUT_AND_GET
	binary entry_rlt(rlt_size, 0);
	memcpy(&entry_rlt[0], rlt, rlt_size);
	cache[key] = entry_t(entry_meta, entry_rlt);
#else
	cache[key] = entry_t(entry_meta, entry_meta);
#endif



	get_time(&end_time);

#ifndef TEST_PUT_AND_GET
	eprintf("put cache use %d us \n", time_elapsed_in_us(&start_time, &end_time));
#endif

	//eprintf("In get_cache the Map size is %d\n", cache.size());


}