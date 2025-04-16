#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage.h"

const char UNIT_PACK[] = "unit";
const char PERSISTENCE_PACK[] = "persistence";
const char STRESS_PACK[] = "stress";

const char BEFORE_CRASH_STATE[] = "before_crash";
const char AFTER_CRASH_STATE[] = "after_crash";

const char UNIT_ROOT[] = "unit_root";
const char PERSISTENCE_ROOT[] = "persistence_root";
const char STRESS_ROOT[] = "stress_root";

const char SAMPLE_KEY[] = "caos";
const char SAMPLE_VALUE[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

const char PASSWORD_KEY[] = "password";
const char PASSWORD_VALUE_1[] = "12345678";
const char PASSWORD_VALUE_2[] = "rejuvenate prodders rooted flowmeters fraudful";

const char STRESS_SAMPLE_KEY[] = "cat";
const char STRESS_SAMPLE_VALUE[] = "meow";

const uint64_t STRESS_KEYS_COUNT = 1e3;
const uint64_t STRESS_VALUES_COUNT = 1e3;

void test_set(storage_t* storage) {
    version_t actual_version = storage_set(storage, SAMPLE_KEY, SAMPLE_VALUE);
    version_t expected_version = 1;

    assert(actual_version == expected_version && "New key version must be equal to 1!");

    actual_version = storage_set(storage, SAMPLE_KEY, SAMPLE_VALUE);
    expected_version = 2;

    assert(actual_version == expected_version && "Version number must be incremented after each set!");
}

void test_get(storage_t* storage) {
    version_t set_version = storage_set(storage, SAMPLE_KEY, SAMPLE_VALUE);

    char returned_value[MAX_VALUE_SIZE];
    version_t get_version = storage_get(storage, SAMPLE_KEY, returned_value);

    assert(get_version != 0 && "Can't find the key that was set!");
    assert(get_version == set_version && "Set and get version mismatch!");

    assert((!strcmp(returned_value, SAMPLE_VALUE)) && "Return value mismatch!");
}

void test_get_by_version(storage_t* storage) {
    version_t old_set_version = storage_set(storage, SAMPLE_KEY, SAMPLE_VALUE);
    version_t new_set_version = storage_set(storage, SAMPLE_KEY, SAMPLE_VALUE);

    char returned_value[MAX_VALUE_SIZE];
    version_t get_version = storage_get_by_version(storage, SAMPLE_KEY, old_set_version, returned_value);

    assert(get_version != 0 && "Failed to get old key value!");
    assert(get_version == old_set_version && "Got wrong version number!");

    get_version = storage_get_by_version(storage, SAMPLE_KEY, new_set_version, returned_value);

    assert(get_version == new_set_version && "Got wrong version number!");
}

void test_scenario_1(storage_t* storage) {
    // Check if password doesn't exits
    char password[MAX_VALUE_SIZE];
    version_t get_version = storage_get(storage, PASSWORD_KEY, password);
    assert(get_version == 0 && "Non-existent key version must be 0!");

    // Double check
    get_version = storage_get(storage, PASSWORD_KEY, password);
    assert(get_version == 0 && "Non-existent key version ALWAYS must be 0!");

    // Nice, now I can save my bank password
    version_t old_set_version = storage_set(storage, PASSWORD_KEY, PASSWORD_VALUE_1);
    assert(old_set_version == 1 && "New key version must be equal to 1!");

    // Check if it saved
    storage_get(storage, PASSWORD_KEY, password);
    assert((!strcmp(password, PASSWORD_VALUE_1)) && "Return value mismatch after first set!");

    // Seems like my password isn't strong enough, I need to use another one
    storage_set(storage, PASSWORD_KEY, PASSWORD_VALUE_2);

    // Check if it saved
    storage_get(storage, PASSWORD_KEY, password);
    assert((!strcmp(password, PASSWORD_VALUE_2)) && "Return value mismatch after new set!");

    // What was my old password?
    storage_get_by_version(storage, PASSWORD_KEY, old_set_version, password);
    assert((!strcmp(password, PASSWORD_VALUE_1)) && "Old return value mismatch after new set!");
}

void test_scenario_2(storage_t* storage) {
    /*
      Oh no, my computer crashed(((
      I forgot my bank password(((
      Fortunately, I've got an excellent persistent storage
    */

    char password[MAX_VALUE_SIZE];
    version_t get_version = storage_get(storage, PASSWORD_KEY, password);

    assert(get_version != 0 && "Failed to found key after program crash!");
    assert(get_version == 2 && "Wrong key version after program crash!");

    assert((!strcmp(password, PASSWORD_VALUE_2)) && "Wrong key value after program crash!");
}

void test_many_keys(storage_t* storage) {
    for (uint64_t i = 100; i < STRESS_KEYS_COUNT + 100; ++i) {
        char key[64];
        snprintf(key, 64, "%ld", i);

        version_t set_version = storage_set(storage, key, STRESS_SAMPLE_VALUE);
        assert(set_version == 1 && "New key version must be equal to 1!");

        char returned_value[MAX_VALUE_SIZE];
        version_t get_version = storage_get(storage, key, returned_value);

        assert(get_version != 0 && "Failed to found key after set!");
        assert((!strcmp(returned_value, STRESS_SAMPLE_VALUE)) && "Returned value mismatch!");
    }
}

void test_many_values(storage_t* storage) {
    for (uint64_t i = 0; i < STRESS_VALUES_COUNT; ++i) {
        version_t set_version = storage_set(storage, STRESS_SAMPLE_KEY, STRESS_SAMPLE_VALUE);
    }

    for (version_t version = 1; version <= STRESS_VALUES_COUNT; ++version) {
        char returned_value[MAX_VALUE_SIZE];
        version_t get_version = storage_get_by_version(storage, STRESS_SAMPLE_KEY, version, returned_value);
        
        assert(get_version == version && "Version mismatch!");
        assert((!strcmp(returned_value, STRESS_SAMPLE_VALUE)) && "Returned value mismatch!");
    }
}

void run_unit_tests() {
    storage_t storage;
    char root_path[PATH_MAX];
    char* realpath_result = realpath(UNIT_ROOT, root_path);
    assert(realpath_result == root_path);

    storage_init(&storage, root_path);

    test_set(&storage);
    test_get(&storage);
    test_get_by_version(&storage);

    storage_destroy(&storage);
}

void run_persistence_tests(bool after_crash) {
    storage_t storage;
    char root_path[PATH_MAX];
    char* realpath_result = realpath(PERSISTENCE_ROOT, root_path);
    assert(realpath_result == root_path);

    storage_init(&storage, root_path);

    if (!after_crash) {
        test_scenario_1(&storage);
    } else {
        test_scenario_2(&storage);
    }

    storage_destroy(&storage);
}

void run_stress_tests() {
    storage_t storage;
    char root_path[PATH_MAX];
    char* realpath_result = realpath(STRESS_ROOT, root_path);
    assert(realpath_result == root_path);

    storage_init(&storage, root_path);

    test_many_keys(&storage);
    test_many_values(&storage);

    storage_destroy(&storage);
}

void test_cleanup() {
    int fd = open("test_file", O_CREAT | O_RDONLY, 0744);
    close(fd);
    unlink("test_file");

    assert(fd == 3 && "Some file descriptors wasn't closed!");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Missing test pack (unit, persistence, stress)\n");
        return 1;
    }

    if (!strcmp(UNIT_PACK, argv[1])) {
        run_unit_tests();

    } else if (!strcmp(PERSISTENCE_PACK, argv[1])) {
        if (argc < 3) {
            printf("Missing state for persistence test (before_crash, after_crash)\n");
            return 1;
        }

        if (!strcmp(BEFORE_CRASH_STATE, argv[2])) {
            run_persistence_tests(false);

        } else if (!strcmp(AFTER_CRASH_STATE, argv[2])) {
            run_persistence_tests(true);

        } else {
            printf("Unknown state for persistence test\n");
            return 1;
        }

    } else if (!strcmp(STRESS_PACK, argv[1])) {
        run_stress_tests();

    } else {
        printf("Unknown test pack\n");
        return 1;
    }

    test_cleanup();

    return 0;
}
