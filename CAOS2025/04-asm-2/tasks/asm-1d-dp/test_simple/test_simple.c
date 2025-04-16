#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern uint64_t longest_inc_subseq(int64_t* array, uint64_t* help_array, uint64_t size);

size_t test_passed = 0;
size_t test_failed = 0;

void test_simple_helper(int64_t* array, uint64_t size, uint64_t exp_ans) {
  uint64_t help_array[size];

  uint64_t ans = longest_inc_subseq(array, help_array, size);
//  printf("%ld %ld\n", ans, exp_ans);
  if (ans == exp_ans) {
    ++test_passed;
  } else {
    ++test_failed;
  }
}

void test_simple() {
  {
    int64_t array[5] = {0, 1, 2, 3, 4};
    test_simple_helper(array, 5, 5);
  }
  {
    int64_t array[5] = {1, 1, 1, 1, 1};
    test_simple_helper(array, 5, 1);
  }
  {
    int64_t* array;
    test_simple_helper(array, 0, 0);
  }
}

void test_by_input() {
  FILE* input = fopen("input_simple/input.txt", "r");
  FILE* input_answers = fopen("input_simple/input_answers.txt", "r");

  size_t tests_count;
  fscanf(input, "%zu", &tests_count);

  for (size_t i = 0; i < tests_count; ++i) {
    uint64_t size;
    fscanf(input, "%" SCNu64, &size);

    int64_t* array = malloc(size * sizeof(int64_t));
    uint64_t* help_array = malloc(size * sizeof(uint64_t));

    for (uint64_t j = 0; j < size; ++j) {
      fscanf(input, "%" SCNd64, array + j);
    }

    uint64_t ans = longest_inc_subseq(array, help_array, size);

    uint64_t exp_ans;
    fscanf(input_answers, "%" SCNu64, &exp_ans);

    if (ans == exp_ans) {
      ++test_passed;
    } else {
      ++test_failed;
    }

    free(array);
    free(help_array);
  }

  fclose(input);
  fclose(input_answers);
}

int main() {
  test_simple();
  test_by_input();

  printf("Passed %zu tests\n", test_passed);
  printf("Failed %zu tests\n", test_failed);

  if (test_failed > 0) {
    return 1;
  }
  return 0;
}
