#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
   int fd;
} utf8_file_t;

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count);
int utf8_read(utf8_file_t* f, uint32_t* res, size_t count);
utf8_file_t* utf8_fromfd(int fd);
