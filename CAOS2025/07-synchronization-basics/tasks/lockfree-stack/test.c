#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lockfree-stack.h"

void alarm_handler(int signum) {
  printf("    Time limit exceeded :((\n");
  exit(1);
}

#define TEST_WITH_TIMEOUT(name, seconds, expr)                                 \
  do {                                                                         \
    printf("    Run test " #name "...\n");                                     \
    alarm(seconds);                                                            \
    if (0 == (expr)) {                                                         \
      alarm(0);                                                                \
      printf("    Test " #name " PASSED!\n");                                  \
    } else {                                                                   \
      alarm(0);                                                                \
      printf("    Test " #name " FAILED!\n");                                  \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

int test_lifo() {
  lfstack_t stack;
  lfstack_init(&stack);
  for (size_t i = 1; i <= 1000; ++i) {
    if (0 != lfstack_push(&stack, i)) {
      perror("unexpected error during push");
      return 1;
    }
  }
  for (size_t i = 1000; i >= 1; --i) {
    size_t result;
    if (0 != lfstack_pop(&stack, &result)) {
      perror("unexpected error during pop");
      return 1;
    }
    if (result != i) {
      printf("LIFO property is broken!");
      return 1;
    }
  }
  lfstack_destroy(&stack);
  return 0;
}

int test_push_pop_stress() {
  lfstack_t stack;
  lfstack_init(&stack);
  for (size_t i = 1; i <= 1000000; ++i) {
    if (0 != lfstack_push(&stack, i)) {
      perror("unexpected error during push");
      return 1;
    }
    size_t result;
    if (0 != lfstack_pop(&stack, &result)) {
      perror("unexpected error during pop");
      return 1;
    }
    if (result != i) {
      printf("Got something wrong!");
      return 1;
    }
  }
  lfstack_destroy(&stack);
  return 0;
}

void *push_routine(void *args) {
  for (size_t i = 1; i <= 1000; ++i) {
    if (0 != lfstack_push(args, i)) {
      perror("unexpected error during push");
      return (void *)1;
    }
  }
  return NULL;
}

void *pop_routine(void *args) {
  size_t res = 0;
  do {
    if (0 != lfstack_pop(args, &res)) {
      perror("unexpected error during pop");
      return (void *)1;
    }
  } while (res);

  return NULL;
}

int test_multiproducer(size_t n_threads) {
  lfstack_t stack;
  lfstack_init(&stack);
  pthread_t *threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
  for (size_t i = 0; i < n_threads; ++i) {
    pthread_create(&threads[i], NULL, push_routine, &stack);
  }

  for (size_t i = 0; i < n_threads; ++i) {
    void *ret;
    pthread_join(threads[i], &ret);
    if (ret != NULL) {
      return 1;
    }
  }

  if (NULL != pop_routine(&stack)) {
    return 1;
  }
  free(threads);
  lfstack_destroy(&stack);

  return 0;
}

int test_mpsc(size_t n_threads) {
  lfstack_t stack;
  lfstack_init(&stack);
  pthread_t *threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
  for (size_t i = 0; i < n_threads; ++i) {
    pthread_create(&threads[i], NULL, push_routine, &stack);
  }

  if (NULL != pop_routine(&stack)) {
    return 1;
  }

  for (size_t i = 0; i < n_threads; ++i) {
    void *ret;
    pthread_join(threads[i], &ret);
    if (ret != NULL) {
      return 1;
    }
  }

  if (NULL != pop_routine(&stack)) {
    return 1;
  }

  free(threads);
  lfstack_destroy(&stack);

  return 0;
}

int main() {
  signal(SIGALRM, alarm_handler);
  TEST_WITH_TIMEOUT(LIFO, 1, test_lifo());
  TEST_WITH_TIMEOUT(push_pop_stress, 2, test_push_pop_stress());
  TEST_WITH_TIMEOUT(multiproducer_few_threads, 1,
                    test_multiproducer(sysconf(_SC_NPROCESSORS_ONLN)));
  TEST_WITH_TIMEOUT(multiproducer_many_threads, 1000, test_multiproducer(100));
  TEST_WITH_TIMEOUT(mpsc_few_threads, 1,
                    test_mpsc(sysconf(_SC_NPROCESSORS_ONLN)));
  TEST_WITH_TIMEOUT(mpsc_many_threads, 1, test_mpsc(100));
}
