#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

extern int64_t add(int64_t left, int64_t right);

size_t test_passed = 0;
size_t test_failed = 0;

void check_function_add(int64_t first, int64_t second) {
  int64_t expected = first + second;

  if (add(first, second) == expected) {
    ++test_passed;
  } else {
    ++test_failed;
  }
}

void test_input() {
  FILE* input = fopen("input_simple/input_simple.txt", "r");

  size_t number_pair_counter;
  fscanf(input, "%zu", &number_pair_counter);

  for (size_t i = 0; i < number_pair_counter; ++i) {
    int64_t first, second;
    fscanf(input, "%" SCNd64 " " "%" SCNd64, &first, &second);
    check_function_add(first, second);
  }

  fclose(input);
}

int main() {
  test_input();

  printf("===== Simple test =====\n");
  printf("Passed %zu tests\n", test_passed);
  printf("Failed %zu tests\n", test_failed);

  if (test_failed > 0) {
    return 1;
  }
  return 0;
}
