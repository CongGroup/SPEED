#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "config.h"
#include <stdint.h>

// meta data for cache entry
typedef struct metadata_t {
    uint8_t r[RAND_SIZE];
    uint8_t mac[MAC_SIZE];
} metadata;

#define RAW(data) ((uint8_t*)(data))

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