#include "storage.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

void storage_init(storage_t* storage, const char* root_path) {
  storage->root_path = strdup(root_path);
}

void storage_destroy(storage_t* storage) {
  free(storage->root_path);
}

static char* get_filepath(storage_t* storage, storage_key_t key) {
  size_t key_len = strlen(key);
  size_t path_len = strlen(storage->root_path) + 1; // +1 for '/'
  size_t dirs_count = key_len / SUBDIR_NAME_SIZE + (key_len % SUBDIR_NAME_SIZE != 0);
  path_len += dirs_count * (SUBDIR_NAME_SIZE + 1); // +1 for '/'
  path_len += 2; // +1 for filename (or '@' if needed) +1 for '\0'

  char* path = (char*)malloc(path_len);
  strcpy(path, storage->root_path);
  strcat(path, "/");

  for (size_t i = 0; i < key_len; i += SUBDIR_NAME_SIZE) {
    strncat(path, key + i, (key_len - i >= SUBDIR_NAME_SIZE) ? SUBDIR_NAME_SIZE : key_len - i);
    strcat(path, "/");
    char dir_path[strlen(path) + 1];
    strcpy(dir_path, path);
    dir_path[strlen(path) - 1] = '\0';
    mkdir(dir_path, 0777);
  }

  if (key_len % SUBDIR_NAME_SIZE == 0) {
    strcat(path, "@");
  }

  return path;
}

version_t storage_set(storage_t* storage, storage_key_t key, storage_value_t value) {
  char* filepath = get_filepath(storage, key);
  int fd = open(filepath, O_WRONLY | O_CREAT | O_APPEND, 0666);
  free(filepath);

  if (fd == -1) {
    return 0;
  }

  size_t value_len = strlen(value);
  write(fd, &value_len, sizeof(size_t));
  write(fd, value, value_len);

  version_t version = lseek(fd, 0, SEEK_END);
  close(fd);
  return version;
}

version_t storage_get(storage_t* storage, storage_key_t key, returned_value_t returned_value) {
  char* filepath = get_filepath(storage, key);
  int fd = open(filepath, O_RDONLY);
  free(filepath);

  if (fd == -1) {
    return 0;
  }

  version_t last_version = 0;
  size_t value_len;
  ssize_t bytes_read;

  while ((bytes_read = read(fd, &value_len, sizeof(size_t))) > 0) {
    last_version = lseek(fd, 0, SEEK_CUR);
    if (read(fd, returned_value, value_len) != value_len) {
      close(fd);
      return 0;
    }
    returned_value[value_len] = '\0';
  }

  close(fd);
  return last_version;
}

version_t storage_get_by_version(storage_t* storage, storage_key_t key, version_t version, returned_value_t returned_value) {
  char* filepath = get_filepath(storage, key);
  int fd = open(filepath, O_RDONLY);
  free(filepath);

  if (fd == -1) {
    return 0;
  }

  lseek(fd, version - sizeof(size_t), SEEK_SET); // Adjusted to correctly point to the length of the value
  size_t value_len;
  if (read(fd, &value_len, sizeof(size_t)) != sizeof(size_t)) {
    close(fd);
    return 0;
  }

  if (read(fd, returned_value, value_len) != value_len) {
    close(fd);
    return 0;
  }
  returned_value[value_len] = '\0';

  close(fd);
  return version;
}