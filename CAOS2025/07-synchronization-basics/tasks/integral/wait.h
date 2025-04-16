#pragma once

#include <limits.h>
#include <stdatomic.h>
#include <stdint.h>

#if __linux__

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>

static inline int PlatformWait(uint32_t *ptr, uint32_t old) {
  return syscall(SYS_futex, ptr, FUTEX_WAIT_PRIVATE, old, NULL, NULL, 0);
}

static inline int PlatformWakeOne(uint32_t *ptr) {
  return syscall(SYS_futex, (uint32_t*)ptr, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
}

static inline int PlatformWakeAll(uint32_t *ptr) {
  return syscall(SYS_futex, (uint32_t*)ptr, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
}

#elif __APPLE__

extern int __ulock_wait(uint32_t operation, void* addr, uint64_t value, uint32_t timeout);
extern int __ulock_wake(uint32_t operation, void* addr, uint64_t wake_value);

#define UL_COMPARE_AND_WAIT_ONE 1
#define UL_COMPARE_AND_WAIT_ALL	UL_COMPARE_AND_WAIT_ONE | 0x00000100

static inline int PlatformWait(uint32_t *ptr, uint32_t old) {
  return __ulock_wait(UL_COMPARE_AND_WAIT_ONE, ptr, old, 0);
}

static inline int PlatformWakeOne(uint32_t *ptr) {
  return __ulock_wake(UL_COMPARE_AND_WAIT_ONE, ptr, 0);
}

static inline int PlatformWakeAll(uint32_t *ptr) {
  return __ulock_wake(UL_COMPARE_AND_WAIT_ALL, ptr, 0);
}

#else

#error "Futex is not implemented on current platform"

#endif

static inline void atomic_wait_explicit(_Atomic(uint32_t)* ptr, uint32_t old, int mo) {
  while (atomic_load_explicit(ptr, mo) == old) {
    PlatformWait((uint32_t*)ptr, old);
  }
}

static inline void atomic_wait(_Atomic(uint32_t) *ptr, uint32_t old) {
  return atomic_wait_explicit(ptr, old, __ATOMIC_SEQ_CST);
}

static inline void atomic_notify_one(_Atomic(uint32_t) *ptr) {
  PlatformWakeOne((uint32_t*)ptr);
}

static inline void atomic_notify_all(_Atomic(uint32_t) *ptr) {
  PlatformWakeAll((uint32_t*)ptr);
}
