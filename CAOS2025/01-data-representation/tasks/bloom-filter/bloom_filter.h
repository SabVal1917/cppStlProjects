#include <stdbool.h>
#include <stdint.h>

typedef uint64_t (*hash_fn_t)(const char*, uint64_t, uint64_t);

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed);

struct BloomFilter {
    uint64_t* set;
    uint64_t set_size;
    hash_fn_t hash_fn;
    uint64_t hash_fn_count;
};
typedef const char* Key;

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count);

void bloom_destroy(struct BloomFilter* bloom_filter);

void bloom_insert(struct BloomFilter* bloom_filter, Key key);

bool bloom_check(struct BloomFilter* bloom_filter, Key key);
