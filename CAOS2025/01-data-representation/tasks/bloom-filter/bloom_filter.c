#include "bloom_filter.h"
#include <stddef.h>
#include <stdlib.h>

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
  uint64_t hash = 0;
  uint64_t power = seed;
  while (*str != 0) {
    hash = ((seed * (*str)) % modulus + hash) % modulus;
    ++str;
    seed = (power * seed) % modulus;
  }
  return hash;
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size,
                hash_fn_t hash_fn, uint64_t hash_fn_count) {
  bloom_filter->set_size = set_size;
  bloom_filter->hash_fn = hash_fn;
  bloom_filter->hash_fn_count = hash_fn_count;
  bloom_filter->set = (uint64_t*)calloc(set_size, sizeof(uint64_t));
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
  free(bloom_filter->set);
  bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
    bloom_filter->set[hash / 64] |= (1ULL << (hash % 64));
  }
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
  for (uint64_t i = 0; i < bloom_filter->hash_fn_count; ++i) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, i);
    if ((bloom_filter->set[hash / 64] & (1ULL << (hash % 64))) == 0) {
      return false;
    }
  }
  return true;
}