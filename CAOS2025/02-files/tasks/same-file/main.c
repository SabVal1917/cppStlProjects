#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const size_t PATH_MAX = 1024;

bool is_same_file(const char* lhs_path, const char* rhs_path) {
  char realpath_lhs[PATH_MAX];
  char realpath_rhs[PATH_MAX];
  if (realpath(lhs_path, realpath_lhs) == NULL ||
      realpath(rhs_path, realpath_rhs) == NULL) {
    return false;
  }
  struct stat lhs_stat, rhs_stat;
  if (stat(realpath_lhs, &lhs_stat) == -1 ||
      stat(realpath_rhs, &rhs_stat) == -1) {
    return false;
  }
  if (lhs_stat.st_ino == rhs_stat.st_ino) {
    return true;
  }
  return false;
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    return 1;
  }
  const char* first_file = argv[1];
  const char* second_file = argv[2];
  if (is_same_file(first_file, second_file)) {
    printf("%s", "yes");
  } else {
    printf("%s", "no");
  }
  return 0;
}
