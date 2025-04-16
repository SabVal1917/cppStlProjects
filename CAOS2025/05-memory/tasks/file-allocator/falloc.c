#include "falloc.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

const uint64_t cLastBits = 63;
void falloc_init(file_allocator_t* allocator, const char* filepath,
                 uint64_t allowed_page_count) {
  //  need stat from file
  struct stat fst;
  bool file_existed = false;
  int fd;
  if (stat(filepath, &fst) == -1) {
    //    no such file found => lets create this file
    if (errno == 2) {
      fd = open(filepath, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    } else {
      //      smth strange has appeared
      return;
    }
  } else {
    file_existed = true;
    fd = open(filepath, O_RDWR);
  }
  if (fd == -1) {
    return;
  }
  // truncate file
  int flag_ftruncate =
      ftruncate(fd, PAGE_MASK_SIZE + PAGE_SIZE * allowed_page_count);
  if (flag_ftruncate == -1) {
    close(fd);
    return;
  }
  // mmap file
  void* addr = mmap(NULL, PAGE_MASK_SIZE + PAGE_SIZE * allowed_page_count,
                    PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    close(fd);
    return;
  }
  // finally, can init alloc, so we want to init continious memory
  allocator->fd = fd;
  allocator->curr_page_count = 0;
  allocator->allowed_page_count = allowed_page_count;
  allocator->page_mask = addr;
  // we will have : first 512b - for page_mask, after that we will have 4096b -
  // for page pointers
  allocator->base_addr = addr + PAGE_MASK_SIZE;
  if (file_existed) {
    // хотим просто узнать число текущих страниц
    for (uint64_t i = 0; i < allowed_page_count; ++i) {
      uint64_t page_mask = allocator->page_mask[i >> 6];
      uint64_t page_bit = (1ULL << (i & cLastBits));
      if ((page_bit & page_mask) != 0) {
        allocator->curr_page_count += 1;
      }
    }
  } else {
    memset(allocator->page_mask, 0, PAGE_MASK_SIZE);
  }
}

void falloc_destroy(file_allocator_t* allocator) {
  msync(allocator->page_mask,
        PAGE_MASK_SIZE + PAGE_SIZE * allocator->allowed_page_count, MS_SYNC);
  munmap(allocator->page_mask,
         PAGE_MASK_SIZE + PAGE_SIZE * allocator->allowed_page_count);
  allocator->page_mask = NULL;
  allocator->base_addr = NULL;
  close(allocator->fd);
}

void* falloc_acquire_page(file_allocator_t* allocator) {
  for (uint64_t i = 0; i < allocator->allowed_page_count; ++i) {
    uint64_t page_mask = allocator->page_mask[i >> 6];
    uint64_t page_bit = (1ULL << (i & cLastBits));
    if ((page_bit & page_mask) == 0) {
      allocator->page_mask[i >> 6] |= page_bit;
      allocator->curr_page_count += 1;
      return allocator->base_addr + i * PAGE_SIZE;
    }
  }
  return NULL;
}

void falloc_release_page(file_allocator_t* allocator, void** addr) {
  if (addr == NULL) {
    return;
  }
  uint64_t indx = (*addr - allocator->base_addr) >> 12;
  uint64_t page_bit_reverse = ~(1ULL << (indx & cLastBits));
  allocator->page_mask[indx >> 6] &= page_bit_reverse;
  allocator->curr_page_count -= 1;
  *addr = NULL;
}
