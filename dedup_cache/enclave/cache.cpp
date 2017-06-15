#include <utility>
#include <unordered_map>
#include <cstring>

#define TAG_SIZE 256
#define RAND_SIZE 256

typedef std::string binary;
typedef std::pair<binary, binary> entry_t;
typedef std::unordered_map<binary, entry_t> cache_t;

cache_t cache;

int ecall_cache_get(char *tag,
                    char *rlt, int rlt_size,
                    char *r) 
{
    cache_t::const_iterator it= cache.find(binary(tag, TAG_SIZE));
    // cache miss
    if (it == cache.end()) {
        return 0;
    }
    // cache hit
    else {
        const entry_t &entry = it->second;

        // copy computation result, rlt_size >= entry.first.size()
        memcpy(rlt, entry.first.data(), entry.first.size());
        // copy randomness
        memcpy(r, entry.second.data(), RAND_SIZE);

        return 1;
    }
}

void ecall_cache_put(char *tag,
                     char *rlt, int rlt_size,
                     char *r)
{
    cache[binary((char *)tag, TAG_SIZE)] =
        entry_t(binary(rlt, rlt_size), binary(r, RAND_SIZE));
}