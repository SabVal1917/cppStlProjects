#pragma once

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lockfree-stack.h"

typedef struct lfnode {
  atomic_uintptr_t next;
  uintptr_t value;
} lfnode_t;

typedef struct lfstack {
  atomic_uintptr_t top;
} lfstack_t;

int lfstack_init(lfstack_t* stack) {
  stack->top = (atomic_uintptr_t)NULL;
  return 0;
}

int lfstack_push(lfstack_t* stack, uintptr_t value) {
  lfnode_t* new_node = (lfnode_t*)malloc(sizeof(lfnode_t));
  if (new_node == NULL) {
    return EXIT_FAILURE;
  }
  new_node->value = value;
  atomic_uintptr_t prev_top = stack->top;
  new_node->next = prev_top;
  while (!atomic_compare_exchange_strong(&stack->top, &prev_top,
                                         (uintptr_t)new_node)) {
    prev_top = stack->top;
    new_node->next = prev_top;
  }
  return 0;
}

int lfstack_pop(lfstack_t* stack, uintptr_t* value) {
  if (stack->top == (uintptr_t)NULL) {
    *value = 0;
    return 0;
  }
  atomic_uintptr_t top = stack->top;
  while (top != (uintptr_t)NULL &&
         !atomic_compare_exchange_strong(&stack->top, &top, ((lfnode_t*)top)->next)) {
  }
  *value = ((lfnode_t*)top)->value;
  free((lfnode_t*)top);
  return 0;
}

int lfstack_destroy(lfstack_t* stack) {
  stack->top = (atomic_uintptr_t)NULL;
  return 0;
}
