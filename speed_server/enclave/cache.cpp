#include "Enclave_t.h"
#include "sgx_trts.h"

#include "../../common/data_type.h"

#include <cstring>
#include <map>
//#include <unordered_map>
#include <vector>
#include <utility> // pair

#include "sysutils.h"

typedef std::vector<uint8_t> binary;
typedef std::pair<binary, binary> entry_t;
// TODO: replace with unordered_map
// We need to know map max size to use unordered_map
//typedef std::unordered_map<binary, entry_t> cache_t;
typedef std::map<binary, entry_t> cache_t;

cache_t cache;

//void printBlock(const void* src, int size)
//{
//	for (int i = 0; i < size; ++i)
//	{
//		eprintf("%.2X ", *((const unsigned char*)src + i));
//		if (i % 16 == 15 || i == size - 1)
//			eprintf("\n");
//	}
//}

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
	eprintf(" %d us, and ", time_elapsed_in_us(&start_time, &end_time));
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
	eprintf(" %d us, and ", time_elapsed_in_us(&start_time, &end_time));
#endif

	//eprintf("In get_cache the Map size is %d\n", cache.size());


}