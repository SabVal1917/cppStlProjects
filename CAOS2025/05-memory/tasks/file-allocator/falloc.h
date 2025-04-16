#include <stdint.h>


enum {
    PAGE_SIZE = 4096,
    PAGE_MASK_SIZE = 512
};

typedef struct {
    int fd;
    void* base_addr;
    uint64_t* page_mask;
    uint64_t curr_page_count;
    uint64_t allowed_page_count;
} file_allocator_t;

void falloc_init(file_allocator_t* allocator, const char* filepath, uint64_t allowed_page_count);

void falloc_destroy(file_allocator_t* allocator);

void* falloc_acquire_page(file_allocator_t* allocator);

void falloc_release_page(file_allocator_t* allocator, void** addr);
