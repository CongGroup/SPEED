#include "Enclave_t.h"
#include "sgx_trts.h"

#include "../../common/data_type.h"

#include <cstring>
#include <map>
#include <vector>
#include <utility> // pair

typedef std::vector<uint8_t> binary;
typedef std::pair<binary, binary> entry_t;
// TODO: replace with unordered_map
typedef std::map<binary, entry_t> cache_t;

cache_t cache;

void ecall_cache_get(const uint8_t *tag,
                     uint8_t *meta,
                     uint8_t *rlt, int expt_size,
                     int *true_size)
{
    binary key(TAG_SIZE, 0);
    memcpy(&key[0], tag, TAG_SIZE);

    cache_t::const_iterator it = cache.find(key);
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
        memcpy(rlt, &entry.second[0], (*true_size > expt_size) ? expt_size : *true_size);
    }
}

void ecall_cache_put(const uint8_t *tag,
                     const uint8_t *meta,
                     const uint8_t *rlt, int rlt_size)
{
    binary key(TAG_SIZE, 0);
    memcpy(&key[0], tag, TAG_SIZE);

    binary entry_meta(sizeof(metadata), 0);
    memcpy(&entry_meta[0], meta, sizeof(metadata));

    binary entry_rlt(rlt_size);
    memcpy(&entry_rlt[0], rlt, rlt_size);

    cache[key] = entry_t(entry_meta, entry_rlt);
}