#define _GNU_SOURCE
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "integral.h"

void alarm_handler(int signum) {
  printf("    Time limit exceeded :((\n");
  exit(1);
}

#define TEST_WITH_TIMEOUT(name, seconds, expr) \
  do {                                         \
    printf("    Run test " #name "...\n");     \
    alarm(seconds);                            \
    if (0 == (expr)) {                         \
      alarm(0);                                \
      printf("    Test " #name " PASSED!\n");  \
    } else {                                   \
      alarm(0);                                \
      printf("    Test " #name " FAILED!\n");  \
      exit(1);                                 \
    }                                          \
  } while (0)

#define LIKELY(x) __builtin_expect(!!(x), 1)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)

#define CHECK_OK(expr, msg) \
  if (UNLIKELY((expr))) {   \
    perror((msg));          \
    return 1;               \
  }

const field_t EPS = 1e-7;

int test_single_common(size_t threads_num, func_t* func, field_t left_bound,
                       field_t right_bound, field_t expected) {
  par_integrator_t integrator;
  CHECK_OK(par_integrator_init(&integrator, threads_num),
           "can't init integrator");
  CHECK_OK(
      par_integrator_start_calc(&integrator, func, left_bound, right_bound),
      "can't start calculation");
  double result;
  CHECK_OK(par_integrator_get_result(&integrator, &result), "can't get result");
  CHECK_OK(par_integrator_destroy(&integrator), "can't destroy integrator");
  printf("        Expected %.10f, got %.10f\n", expected, result);
  field_t err = result - expected;
  err = err >= 0 ? err : -err;
  return (err > EPS);
}

double flat_func(double x) { return 1; }
int test_single_flat(size_t threads_num) {
  return test_single_common(threads_num, flat_func, -1, 1, 2);
}

int test_single_sin(size_t threads_num) {
  return test_single_common(threads_num, sin, 0, M_PI_2, 1);
}

double d_arctan_func(double x) { return 1 / (1 + x * x); }
int test_single_pi_4(size_t threads_num) {
  return test_single_common(threads_num, d_arctan_func, 0, 1, M_PI_4);
}

int test_several(size_t threads_num) {
  par_integrator_t integrator;
  CHECK_OK(par_integrator_init(&integrator, threads_num),
           "can't init integrator");
  {
    CHECK_OK(par_integrator_start_calc(&integrator, flat_func, -1, 1),
             "can't start calculation");
    double result, expected = 2;
    CHECK_OK(par_integrator_get_result(&integrator, &result),
             "can't get result");
    printf("        Expected %.10f, got %.10f\n", expected, result);
    field_t err = result - expected;
    err = err >= 0 ? err : -err;
    if (err > EPS) {
      return 1;
    }
  }
  {
    CHECK_OK(par_integrator_start_calc(&integrator, sin, 0, M_PI_2),
             "can't start calculation");
    double result, expected = 1;
    CHECK_OK(par_integrator_get_result(&integrator, &result),
             "can't get result");
    printf("        Expected %.10f, got %.10f\n", expected, result);
    field_t err = result - expected;
    err = err >= 0 ? err : -err;
    if (err > EPS) {
      return 1;
    }
  }
  {
    CHECK_OK(par_integrator_start_calc(&integrator, d_arctan_func, 0, 1),
             "can't start calculation");
    double result, expected = M_PI_4;
    CHECK_OK(par_integrator_get_result(&integrator, &result),
             "can't get result");
    printf("        Expected %.10f, got %.10f\n", expected, result);
    field_t err = result - expected;
    err = err >= 0 ? err : -err;
    if (err > EPS) {
      return 1;
    }
  }
  CHECK_OK(par_integrator_destroy(&integrator), "can't destroy integrator");
  return 0;
}

int test_stress_several(size_t threads_num) {
  par_integrator_t integrator;
  CHECK_OK(par_integrator_init(&integrator, threads_num),
           "can't init integrator");
  for (field_t i = 1; i < 1000; ++i) {
    CHECK_OK(par_integrator_start_calc(&integrator, flat_func, -1 / i, 1 / i),
             "can't start calculation");
    double result, expected = 2 / i;
    CHECK_OK(par_integrator_get_result(&integrator, &result),
             "can't get result");
    field_t err = result - expected;
    err = err >= 0 ? err : -err;
    if (err > EPS) {
      printf("        Expected %.10f, got %.10f\n", expected, result);
      return 1;
    }
  }
  CHECK_OK(par_integrator_destroy(&integrator), "can't destroy integrator");
  return 0;
}

int test_multi_integrators(size_t threads_num) {
  par_integrator_t* integrators =
      malloc(threads_num * sizeof(par_integrator_t));
  for (size_t i = 0; i < threads_num; ++i) {
    CHECK_OK(par_integrator_init(&integrators[i], 1), "can't init integrator");
  }
  for (size_t i = 0; i < threads_num; ++i) {
    CHECK_OK(par_integrator_start_calc(&integrators[i], flat_func, -1, 1),
             "can't start calculation");
  }
  for (size_t i = 0; i < threads_num; ++i) {
    double result;
    CHECK_OK(par_integrator_get_result(&integrators[i], &result),
             "can't get result");
    printf("        Expected %.10f, got %.10f\n", 2.0, result);
    field_t err = result - 2.0;
    err = err >= 0 ? err : -err;
    if (err > EPS) {
      return 1;
    }
  }
  for (size_t i = 0; i < threads_num; ++i) {
    CHECK_OK(par_integrator_destroy(&integrators[i]),
             "can't destroy integrator");
  }
  free(integrators);
  return 0;
}

int main() {
  signal(SIGALRM, alarm_handler);

  size_t threads_num = sysconf(_SC_NPROCESSORS_ONLN);

#if (defined ASAN) || (defined TSAN)
  TEST_WITH_TIMEOUT(SINGLE_FLAT, 2, test_single_flat(threads_num));
  TEST_WITH_TIMEOUT(SINGLE_SIN, 4, test_single_sin(threads_num));
  TEST_WITH_TIMEOUT(SINGLE_PI_4, 4, test_single_pi_4(threads_num));

  TEST_WITH_TIMEOUT(SEVERAL, 6, test_several(threads_num));
  TEST_WITH_TIMEOUT(STRESS_SEVERAL, 10, test_stress_several(threads_num));
  TEST_WITH_TIMEOUT(MULTI_INTEGRATORS, 10, test_multi_integrators(threads_num));
#else
  TEST_WITH_TIMEOUT(SINGLE_FLAT, 1, test_single_flat(threads_num));
  TEST_WITH_TIMEOUT(SINGLE_SIN, 2, test_single_sin(threads_num));
  TEST_WITH_TIMEOUT(SINGLE_PI_4, 2, test_single_pi_4(threads_num));

  TEST_WITH_TIMEOUT(SEVERAL, 4, test_several(threads_num));
  TEST_WITH_TIMEOUT(STRESS_SEVERAL, 7, test_stress_several(threads_num));
  TEST_WITH_TIMEOUT(MULTI_INTEGRATORS, 7, test_multi_integrators(threads_num));
#endif

}
