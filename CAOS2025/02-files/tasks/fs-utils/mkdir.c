#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
  int c;
  int opt_mode = 0;
  int opt_parent = 0;
  mode_t mode = 0777;

  while ((c = getopt(argc, argv, "mp:")) != -1) {
    switch (c) {
      case 'm':
        opt_mode = 1;
        mode = strtoul(optarg, NULL, 8);
        break;
      case 'p':
        opt_parent = 1;
        break;
      default:
        fprintf(stderr, "Usage: %s [-p] [-m mode] path\n", argv[0]);
        return 1;
    }
  }

  for (int i = optind; i < argc; i++) {
    if (opt_parent) {
      char *path = strdup(argv[i]);
      char *p = path;
      char *token = strtok(p, "/");
      while (token != NULL) {
        if (mkdir(token, mode) != 0 && errno != EEXIST) {
          perror("mkdir");
          free(path);
          return 1;
        }
        p = token + strlen(token);
        if (*p != '\0') {
          *p = '/';
          p++;
        }
        token = strtok(NULL, "/");
      }
      free(path);
    } else {
      if (mkdir(argv[i], mode) != 0 && errno != EEXIST) {
        perror("mkdir");
        return 1;
      }
    }
  }

  return 0;
}