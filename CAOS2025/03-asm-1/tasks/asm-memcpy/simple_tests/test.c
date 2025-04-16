#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern void* my_memcpy(void* dest, const void* src, uint32_t count);

size_t test_passed = 0;
size_t test_failed = 0;

void test_string() {
  char string_dest[] = "00000000000000000000000000";
  const char string_src[] = "abcdefghijklmnopqrstuvwxyz";
  const char string_ans[] = "abcdefghijklmnopqrstuvwxyz";
  size_t count = 26;

  my_memcpy(string_dest, string_src, count);

  int dest_check = memcmp(string_dest, string_ans, count);
  int src_check = memcmp(string_src, string_ans, count);

  if (dest_check != 0 || src_check != 0) {
    ++test_failed;
  } else {
    ++test_passed;
  }
}

void test_number() {
  int num_dest = 0;
  int num_src = 75;
  int num_ans = 75;

  my_memcpy(&num_dest, &num_src, sizeof(int));

  if (num_dest != num_ans || num_src != num_ans) {
    ++test_failed;
  } else {
    ++test_passed;
  }
}

void test_return_value() {
  char str_dest[] = "00000";
  char str_src[] = "abcde";
  void* str_ptr = my_memcpy(str_dest, str_src, 5);
  if (str_dest == str_ptr) {
    ++test_passed;
  } else {
    ++test_failed;
  }
}

int main() {
  test_string();
  test_number();
  test_return_value();

  printf("Passed %zu tests\n", test_passed);
  printf("Failed %zu tests\n", test_failed);

  if (test_failed > 0) {
    return 1;
  }
  return 0;
}
