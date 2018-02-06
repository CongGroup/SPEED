#ifndef DEDUP_SERVICE_H
#define DEDUP_SERVICE_H

#include "function.h"

#include <vector>
#include <map>
#include <stdint.h>

typedef std::vector<uint8_t> binary;
typedef std::pair<binary, binary> entry_t;
// TODO: replace with unordered_map
// We need to know map max size to use unordered_map
//typedef std::unordered_map<binary, entry_t> cache_t;
typedef std::map<binary, entry_t> cache_t;

void dedup(Function *func);

void ecall_cache_get(const uint8_t *tag,
	uint8_t *meta,
	uint8_t *rlt, int expt_size,
	int *true_size);

void ecall_cache_put(const uint8_t *tag,
	const uint8_t *meta,
	const uint8_t *rlt, int rlt_size);

#endif