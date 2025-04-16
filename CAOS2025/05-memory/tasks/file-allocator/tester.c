#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "falloc.h"

static const char STORAGE_FILEPATH[] = "storage";
static const char PASSWORD[] = "schnischnaschnappi";
static const char TODO[] = "1. Solve file-allocator\n2. Order pizza";

static const uint64_t ALLOWED_PAGE_COUNT_TINY = 1;
static const uint64_t ALLOWED_PAGE_COUNT_SMALL = 4;
static const uint64_t ALLOWED_PAGE_COUNT_BIG = 4096;

void test_init_destroy() {
    unlink(STORAGE_FILEPATH);

    file_allocator_t allocator;
    falloc_init(&allocator, STORAGE_FILEPATH, ALLOWED_PAGE_COUNT_SMALL);

    assert(allocator.fd > 2 && "Invalid fd!");
    assert(allocator.base_addr != NULL && "Invalid base_addr!");
    assert(allocator.curr_page_count == 0 && "Invalid curr_page_count!");
    assert(allocator.allowed_page_count == ALLOWED_PAGE_COUNT_SMALL && "Invalid allowed_page_count!");
    assert(allocator.page_mask[0] == 0 && "Invalid page_mask!");

    falloc_destroy(&allocator);

    assert(close(allocator.fd) != 0 && "Unclosed fd!");
    assert(allocator.base_addr == NULL && "Unnulled pointer!");
    assert(allocator.page_mask == NULL && "Unnulled pointer!");
}

void test_acquire() {
    unlink(STORAGE_FILEPATH);

    file_allocator_t allocator;
    falloc_init(&allocator, STORAGE_FILEPATH, ALLOWED_PAGE_COUNT_TINY);

    void* addr = falloc_acquire_page(&allocator);
    assert(addr != NULL && "Failed to acquire page!");
    assert(allocator.curr_page_count == 1 && "Only one page must be allocated!");
    assert(allocator.page_mask[0] == 1 && "Invalid page mask!");

    for (char* ptr = (char*)addr; (void*)ptr < addr + PAGE_SIZE; ++ptr) {
        *ptr = 'a';
    }

    void* another_addr = falloc_acquire_page(&allocator);
    assert(another_addr == NULL && "Must be impossible to acquire page!");
    assert(allocator.curr_page_count == 1 && "Only one page must be allocated!");
    assert(allocator.page_mask[0] == 1 && "Invalid page mask!");

    falloc_destroy(&allocator);
}

void test_release() {
    unlink(STORAGE_FILEPATH);

    file_allocator_t allocator;
    falloc_init(&allocator, STORAGE_FILEPATH, ALLOWED_PAGE_COUNT_TINY);

    void* addr = falloc_acquire_page(&allocator);

    falloc_release_page(&allocator, &addr);
    assert(addr == NULL && "Unnulled pointer!");
    assert(allocator.page_mask[0] == 0 && "Invalid page mask!");

    addr = falloc_acquire_page(&allocator);
    assert(addr != NULL && "Failed to acquire released page!");

    falloc_destroy(&allocator);
}

void test_scenario() {
    unlink(STORAGE_FILEPATH);

    file_allocator_t allocator;
    falloc_init(&allocator, STORAGE_FILEPATH, ALLOWED_PAGE_COUNT_BIG);

    // I've lost my password in persistent storage, maybe store it here
    void* password_addr = falloc_acquire_page(&allocator);
    strcpy((char*)password_addr, PASSWORD);
    assert(!strcmp((char*)password_addr, PASSWORD) && "Passwords doesn't match!");

    // What if I allocate all the pages?
    for (uint64_t i = 1; i < ALLOWED_PAGE_COUNT_BIG; ++i) {
        void* addr = falloc_acquire_page(&allocator);
        assert(addr != NULL && "Failed to acquire page!");

        *((char*)addr) = 'a';
    }
    assert(allocator.curr_page_count == allocator.allowed_page_count && "Not all pages are acquired!");

    // I'd like to put my TODO list in random page
    void* random_page_addr = allocator.base_addr + PAGE_SIZE * 1024;
    strcpy((char*)random_page_addr, TODO);
    assert(!strcmp((char*)random_page_addr, TODO) && "That's not my TODO list!");

    // Save for later
    void* random_page_addr_copy = random_page_addr;

    // I did it, so I can forget about it
    falloc_release_page(&allocator, &random_page_addr);
    assert(allocator.curr_page_count + 1 == allocator.allowed_page_count && "Page wasn't released!");

    // Where new page was allocated?
    void* new_random_page_addr = falloc_acquire_page(&allocator);
    assert(new_random_page_addr == random_page_addr_copy && "Acquired page has unexpected address!");
    assert(allocator.curr_page_count == allocator.allowed_page_count && "Not all pages are acquired!");

    // Goodbye, allocator!
    falloc_destroy(&allocator);

    // Wait, I forgot my password again
    falloc_init(&allocator, STORAGE_FILEPATH, ALLOWED_PAGE_COUNT_BIG);

    // Good news: it was on the first page
    password_addr = allocator.base_addr;
    assert(!strcmp((char*)password_addr, PASSWORD) && "Passwords doesn't match!");
    assert(allocator.curr_page_count == allocator.allowed_page_count && "Not all pages are acquired!");

    // I promise that's all
    falloc_destroy(&allocator);
}

int main() {
    test_init_destroy();

    test_acquire();
    test_release();

    test_scenario();

    return 0;
}
