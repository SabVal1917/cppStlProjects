#include "utf8_file.h"

utf8_file_t* utf8_fromfd(int fd) {
  utf8_file_t* ufd = (utf8_file_t*)malloc(sizeof(utf8_file_t));
  if (ufd == NULL) {
    return NULL;
  }
  ufd->fd = fd;
  return ufd;
}

const int c64 = 0x3F;
int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count) {
  size_t written = 0;
  for (size_t i = 0; i < count; ++i) {
    uint32_t ch = str[i];
    char buf[6];
    size_t len;
    if (ch <= 0x7F) {
      buf[0] = ch;
      len = 1;
    } else if (ch <= 0x7FF) {
      buf[0] = (0xC0 | (ch >> 6));
      buf[1] = (0x80 | (ch & c64));
      len = 2;
    } else if (ch <= 0xFFFF) {
      buf[0] = (0xE0 | (ch >> 12));
      buf[1] = (0x80 | ((ch >> 6) & c64));
      buf[2] = (0x80 | (ch & c64));
      len = 3;
    } else if (ch <= 0x1FFFFF) {
      buf[0] = (0xF0 | (ch >> 18));
      buf[1] = (0x80 | ((ch >> 12) & c64));
      buf[2] = (0x80 | ((ch >> 6) & c64));
      buf[3] = (0x80 | (ch & c64));
      len = 4;
    } else if (ch <= 0x3FFFFF) {
      buf[0] = (0xF8 | (ch >> 24));
      buf[1] = (0x80 | ((ch >> 18) & c64));
      buf[2] = (0x80 | ((ch >> 12) & c64));
      buf[3] = (0x80 | ((ch >> 6) & c64));
      buf[4] = (0x80 | (ch & c64));
      len = 5;
    } else {
      buf[0] = (0xFC | (ch >> 30));
      buf[1] = (0x80 | ((ch >> 24) & c64));
      buf[2] = (0x80 | ((ch >> 18) & c64));
      buf[3] = (0x80 | ((ch >> 12) & c64));
      buf[4] = (0x80 | ((ch >> 6) & c64));
      buf[5] = (0x80 | (ch & c64));
      len = 6;
    }
    ssize_t ret = write(f->fd, buf, len);
    if (ret < 0) {
      return -1;
    }
    written += 1;
  }
  return written;
}

int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
  size_t read_count = 0;
  for (size_t i = 0; i < count; ++i) {
    uint8_t buf[6];
    ssize_t ret = read(f->fd, buf, 1);
    if (ret == 0) {
      break;
    }
    if (ret < 0) {
      return -1;
    }
    uint32_t ch;
    if ((buf[0] & 0x80) == 0) {
      ch = buf[0];
    } else if ((buf[0] & 0xE0) == 0xC0) {
      ret = read(f->fd, buf + 1, 1);
      ch = ((buf[0] & 0x1F) << 6) | (buf[1] & c64);
    } else if ((buf[0] & 0xF0) == 0xE0) {
      ret = read(f->fd, buf + 1, 2);
      ch = ((buf[0] & 0x0F) << 12) | ((buf[1] & c64) << 6) | (buf[2] & c64);
    } else if ((buf[0] & 0xF8) == 0xF0) {
      ret = read(f->fd, buf + 1, 3);
      ch = ((buf[0] & 0x07) << 18) | ((buf[1] & c64) << 12) |
           ((buf[2] & c64) << 6) | (buf[3] & c64);
    } else if ((buf[0] & 0xFC) == 0xF8) {
      ret = read(f->fd, buf + 1, 4);
      ch = ((buf[0] & 0x03) << 24) | ((buf[1] & c64) << 18) |
           ((buf[2] & c64) << 12) | ((buf[3] & c64) << 6) | (buf[4] & c64);
    } else if ((buf[0] & 0xFE) == 0xFC) {
      ret = read(f->fd, buf + 1, 5);
      ch = ((buf[0] & 0x01) << 30) | ((buf[1] & c64) << 24) |
           ((buf[2] & c64) << 18) | ((buf[3] & c64) << 12) |
           ((buf[4] & c64) << 6) | (buf[5] & c64);
    } else {
      return -1;
    }

    res[i] = ch;
    read_count++;
  }
  return read_count;
}
