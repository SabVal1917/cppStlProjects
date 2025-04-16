#include "lca.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pcre.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

pid_t get_ppid(pid_t p) {
  char file_path[256];
  sprintf(file_path, "/proc/%d/stat", p);
  FILE* proc_file = fopen(file_path, "r");
  if (proc_file == NULL) {
    return -1;
  }
  pid_t result = -1;
  if (fscanf(proc_file, "%*d %*s %*c %d", &result) != 1) {
    fclose(proc_file);
    fprintf(stderr, "error with reading file %s\n", file_path);
    return -1;
  }
  fclose(proc_file);
  return result;
}

pid_t find_lca(pid_t x, pid_t y) {
  // algo desciption: find all ancestors, and go downwards from root,
  // while ancestors are equal
  pid_t x_ancestors[MAX_TREE_DEPTH];
  pid_t y_ancestors[MAX_TREE_DEPTH];
  pid_t next_x_anc;
  int x_cnt = 0;
  while ((next_x_anc = get_ppid(x)) != x && x_cnt < MAX_TREE_DEPTH) {
    x_ancestors[x_cnt] = x;
    x = next_x_anc;
    x_cnt += 1;
  }
  pid_t next_y_anc;
  int y_cnt = 0;
  while ((next_y_anc = get_ppid(y)) != y && y_cnt < MAX_TREE_DEPTH) {
    y_ancestors[y_cnt] = y;
    y = next_y_anc;
    y_cnt += 1;
  }
  x_cnt -= 1;
  y_cnt -= 1;
  while (x_cnt >= 0 && y_cnt >= 0 && y_ancestors[y_cnt] == x_ancestors[x_cnt]) {
    x_cnt -= 1;
    y_cnt -= 1;
  }
  return x_ancestors[x_cnt + 1];
}
