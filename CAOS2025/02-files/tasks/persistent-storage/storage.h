#ifndef STORAGE_H
#define STORAGE_H

#include <inttypes.h>

static const uint64_t MAX_KEY_SIZE = 4096;
static const uint64_t MAX_VALUE_SIZE = 4096;
static const uint64_t SUBDIR_NAME_SIZE = 2;

typedef struct storage {
    char* root_path;
} storage_t;

typedef const char* storage_key_t;
typedef const char* storage_value_t;
typedef char* returned_value_t;
typedef uint64_t version_t;

void storage_init(storage_t* storage, const char* root_path);

void storage_destroy(storage_t* storage);

version_t storage_set(storage_t* storage, storage_key_t key, storage_value_t value);

version_t storage_get(storage_t* storage, storage_key_t key, returned_value_t returned_value);

version_t storage_get_by_version(storage_t* storage, storage_key_t key, version_t version, returned_value_t returned_value);

#endif
