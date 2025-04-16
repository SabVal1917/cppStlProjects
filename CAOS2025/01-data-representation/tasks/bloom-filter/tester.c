#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "bloom_filter.h"

const char SAMPLE_STRING[] = "floom bilter";
const uint64_t BIG_MODULUS = 1000000007;

const uint64_t EMPTY_SET = 0;
const uint64_t SET_SIZE = 128;
const uint64_t HASH_FN_COUNT = 4;

const uint64_t STRINGS_COUNT = 100;
const char* STRINGS[] = {"eea7e20f", "d7525cc6", "b0185ae9", "d96002c8", "9c045dd8",
                        "48444e04", "e9b62011", "1e8a39ac", "d94c08f1", "44ab925e",
                        "2cdba4e8", "6fa06356", "d566a745", "6a12fd88", "7fac4c00",
                        "e6f52336", "240b0d6a", "6e78a895", "d8ccfa1b", "bbf64f9e",
                        "0b840cd6", "aa8336ef", "4c2589af", "3d44d206", "6e66938d",
                        "69242010", "ae09dc5e", "db001645", "7c3869c3", "8fac110a",
                        "949c3bc8", "985ec640", "620cb066", "eb47944f", "30d92961",
                        "d50f29da", "dcc5650f", "d6128a37", "814cf68e", "56056c53",
                        "1e47b7d2", "a13b5c50", "08ef8642", "b818253c", "c4536897",
                        "265d6548", "eea6093d", "5ae9e910", "99507c7f", "ae52775e",
                        "a1a7ea4e", "500d50a7", "f2675ee1", "89d26401", "5c2537db",
                        "c64872c2", "8dfa0e2a", "e9e88f2f", "8de1f3ce", "feaef061",
                        "95492e4c", "bc717525", "b45326a2", "ce4f29dc", "7c6d5fa7",
                        "c1c82ab6", "b1f4db62", "50c40a76", "cb62ac69", "4d8519b8",
                        "04f3e43e", "dbe8e730", "b09d14f5", "bf9121c5", "54ac90e7",
                        "4f192dba", "fb35c22c", "4b54f7c7", "01b86043", "6c36b6b1",
                        "cadc41de", "327eeb13", "97d4f4dc", "4f7f3152", "6e03c3c2",
                        "7e476a79", "abd481ca", "d0c0710f", "9c4e8381", "a3bc406d",
                        "a3875af1", "c228be73", "3af871a8", "616d1901", "00b3b94f",
                        "b783b553", "1574ab1f", "988b412c", "67ab36d2", "0ea12e7b"};
const uint64_t FALSE_POSITIVE_THRESHOLD = 50;

void test_calc_hash() {
    for (uint64_t i = 0; i < 10; ++i) {
        for (uint64_t j = i + 1; j < 10; ++j) {
            uint64_t first_hash = calc_hash(SAMPLE_STRING, BIG_MODULUS, i);
            uint64_t second_hash = calc_hash(SAMPLE_STRING, BIG_MODULUS, j);
            assert(first_hash != second_hash && "Hash function is too weak!");
        }
    }
}

void test_init_destroy() {
    struct BloomFilter bloom_filter;
    bloom_init(&bloom_filter, SET_SIZE, calc_hash, HASH_FN_COUNT);

    uint64_t set_size = sizeof(bloom_filter.set) / 64;
    assert((set_size <= SET_SIZE / 64 + 1) && "High usage of memory in bitset!");

    bloom_destroy(&bloom_filter);
    assert(bloom_filter.set == NULL && "Destroyed memory can still be accessed!");
}

void test_insert() {
    struct BloomFilter bloom_filter;
    bloom_init(&bloom_filter, SET_SIZE, calc_hash, HASH_FN_COUNT);

    bloom_insert(&bloom_filter, SAMPLE_STRING);
    
    uint64_t mask = 0;
    uint64_t set_elements_count = (bloom_filter.set_size >> 6) + (bloom_filter.set_size % 64 != 0);
    for (uint64_t i = 0; i < set_elements_count; ++i) {
        mask |= bloom_filter.set[i];
    }

    bloom_destroy(&bloom_filter);

    assert(mask != EMPTY_SET && "Set is still empty after insertion!");
}

void test_check() {
    struct BloomFilter bloom_filter;
    bloom_init(&bloom_filter, SET_SIZE, calc_hash, HASH_FN_COUNT);

    bool first_check_result = bloom_check(&bloom_filter, SAMPLE_STRING);
    bloom_insert(&bloom_filter, SAMPLE_STRING);
    bool second_check_result = bloom_check(&bloom_filter, SAMPLE_STRING);

    bloom_destroy(&bloom_filter);

    assert(!first_check_result && "Found a key in empty set!");
    assert(second_check_result && "No key found after insertion!");
}

void test_scenario() {
    struct BloomFilter bloom_filter;
    bloom_init(&bloom_filter, SET_SIZE, calc_hash, HASH_FN_COUNT);

    uint64_t false_positive_count = 0;
    for (uint64_t i = 0; i < STRINGS_COUNT; ++i) {
        bool check_result = bloom_check(&bloom_filter, STRINGS[i]);
        if (check_result) {
            ++false_positive_count;
        }
        bloom_insert(&bloom_filter, STRINGS[i]);
    }

    bloom_destroy(&bloom_filter);

    assert(false_positive_count <= FALSE_POSITIVE_THRESHOLD && "False positive rate is too high!");
}

int main(int argc, char* argv[]) {
    test_calc_hash();

    test_init_destroy();    
    test_insert();
    test_check();
    
    test_scenario();

    return 0;
}
