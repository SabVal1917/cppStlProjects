#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const size_t BUFFSIZE = 4096;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    char* buffer = "error with input data";
    write(STDERR_FILENO, buffer, sizeof(buffer));
    return 1;
  }
  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  char buffer[BUFFSIZE];

  while (1) {
    ssize_t ret_val = read(fd, buffer, BUFFSIZE);
    if (ret_val == 0) {
      continue;
    }
    if (ret_val == -1) {
      perror("open");
      close(fd);
      return 1;
    }
    if (write(STDOUT_FILENO, buffer, ret_val) == -1) {
      perror("write");
      close(fd);
      return 1;
    }
  }
  close(fd);
  return 0;
}