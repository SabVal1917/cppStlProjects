#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

int rm(const char *path, int is_recursive) {
  struct stat st;
  if (stat(path, &st) == -1) {
    perror("stat");
    return -1;
  }

  if (S_ISLNK(st.st_mode)) {
    if (unlink(path) == -1) {
      perror("unlink");
      return -1;
    }
  } else if (S_ISDIR(st.st_mode)) {
    if (!is_recursive) {
      fprintf(stderr, "rm: cannot remove directory '%s': No such file or directory\n", path);
      return -1;
    }
  } else {
    if (unlink(path) == -1) {
      perror("unlink");
      return -1;
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  int c;
  int opt_recursive = 0;

  while ((c = getopt(argc, argv, "r")) != -1) {
    switch (c) {
      case 'r':
        opt_recursive = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-r] path\n", argv[0]);
        return 1;
    }
  }

  for (int i = optind; i < argc; i++) {
    if (rm(argv[i], opt_recursive) != 0) {
      return 1;
    }
  }

  return 0;
}