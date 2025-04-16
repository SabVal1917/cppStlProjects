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

#define MAX_LINE_LENGTH 4096

void fprocess(const char* filepath, const pcre* pattern,
              const pcre_extra* pattern_extra) {
  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    perror("Error with opening file");
    return;
  }
  struct stat file_stat;
  if (fstat(fd, &file_stat) == -1) {
    perror("Error with getting stat file");
    return;
  }
  if (S_ISDIR(file_stat.st_mode) || file_stat.st_size == 0) {
    close(fd);
    return;
  }
  // mmap data
  char* data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    close(fd);
    perror("mapping failed");
    return;
  }
  // считываем построчно
  int start = 0;
  int line_num = 0;
  for (int i = 0; i < file_stat.st_size; ++i) {
    // end_of_str;
    if (data[i] == '\n' || i == file_stat.st_size - 1) {
      line_num += 1;
      int len_block = i - start;
      if (len_block > MAX_LINE_LENGTH) {
        len_block = MAX_LINE_LENGTH;
      }
      char chunk[MAX_LINE_LENGTH + 1];
      strncpy(chunk, data + start, len_block);
      chunk[len_block] = '\0';
      int matching_arr[30];
      int pattern_matching = pcre_exec(pattern, pattern_extra, chunk, len_block,
                                       0, 0, matching_arr, 30);
      if (pattern_matching >= 0) {
        printf("%s:%d: %s\n", filepath, line_num, chunk);
      }
      start = i + 1;
    }
  }
  munmap(data, file_stat.st_size);
  close(fd);
}

void tdir(const char* dir_path, const pcre* pattern,
          const pcre_extra* pattern_extra) {
  struct stat f_stat;
  if (lstat(dir_path, &f_stat) == -1) {
    return;
  }
  if (!S_ISDIR(f_stat.st_mode)) {
    fprocess(dir_path, pattern, pattern_extra);
    return;
  }

  DIR* dir = opendir(dir_path);
  if (dir == NULL) {
    perror("Error with an opening directory");
    return;
  }
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    char* cur_dir_name = entry->d_name;  // given path
    // skip cur_dir and parent div
    if (strcmp(cur_dir_name, ".") == 0 || strcmp(cur_dir_name, "..") == 0) {
      continue;
    }
    char new_path[PATH_MAX];
    snprintf(new_path, PATH_MAX, "%s/%s", dir_path, cur_dir_name);
    struct stat cur_fstat;
    if (stat(new_path, &cur_fstat) == -1) {
      perror("Error with a constructing file");
      return;
    }
    if (S_ISDIR(cur_fstat.st_mode)) {
      tdir(new_path, pattern, pattern_extra);
    } else {
      fprocess(new_path, pattern, pattern_extra);
    }
  }
  closedir(dir);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "incorrect data format int %s", argv[0]);
    return EXIT_FAILURE;
  }
  const char* regex = argv[1];
  const char* dir = argv[2];
  const char* pcre_error;
  int pcre_error_offset;
  pcre* pattern =
      pcre_compile(regex, PCRE_UTF8, &pcre_error, &pcre_error_offset, NULL);
  if (pattern == NULL) {
    fprintf(stderr, "PCRE compilation failed at offset %d: %s\n",
            pcre_error_offset, pcre_error);
    return EXIT_FAILURE;
  }
  pcre_extra* pattern_extra = pcre_study(pattern, 0, &pcre_error);
  if (pcre_error) {
    fprintf(stderr, "PCRE study failed %s\n", pcre_error);
    return EXIT_FAILURE;
  }

  // all paterns have been compiled correctly, now traverse dir
  tdir(dir, pattern, pattern_extra);
  pcre_free(pattern);
  pcre_free_study(pattern_extra);
  return EXIT_SUCCESS;
}
