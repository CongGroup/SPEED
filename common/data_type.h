#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "config.h"
#include <stdint.h>

// for all sources except enclave boundary
typedef uint8_t byte;

// meta data for cache entry
typedef struct metadata_t {
    byte r[RAND_SIZE];
    byte enc_key[ENC_KEY_SIZE];
    byte mac[MAC_SIZE];
} metadata;

// timing
typedef struct hrtime_t {
    long second;
    long nanosecond;
} hrtime;

inline long time_elapsed_in_us(const hrtime *start, const hrtime *end) {
    return (end->second - start->second) * 1000000.0 +
        (end->nanosecond - start->nanosecond) / 1000.0;
}

#endif