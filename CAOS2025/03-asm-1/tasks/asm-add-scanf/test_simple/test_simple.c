#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

extern int64_t add_scanf();

size_t test_passed = 0;
size_t test_failed = 0;

void check_function_add(int64_t first, int64_t second) {
  int64_t expected = first + second;

  if (add_scanf() == expected) {
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
  int input_fd = open("input_simple/input_simple_scan.txt", O_RDONLY);
  if (input_fd == -1) {
    perror("open");
    return 1;
  }
  int dup2_result = dup2(input_fd, STDIN_FILENO);
  if (dup2_result == -1) {
    perror("dup2");
    return 1;
  }

  test_input();

  printf("===== Simple test =====\n");
  printf("Passed %zu tests\n", test_passed);
  printf("Failed %zu tests\n", test_failed);

  if (test_failed > 0) {
    return 1;
  }
  return 0;
}
