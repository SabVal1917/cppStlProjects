#include "utf8_file.h"

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in < 0) {
        fprintf(stderr, "open failed: %s\n", strerror(errno));
        return 1;
    }

    int fd_out = open(argv[2], O_WRONLY | O_CREAT, 0777);
    if (fd_out < 0) {
        fprintf(stderr, "open failed: %s\n", strerror(errno));
        return 1;
    }

    utf8_file_t* fin = utf8_fromfd(fd_in);
    if (fin == NULL) {
        fprintf(stderr, "no memory\n");
        return 1;
    }

    utf8_file_t* fout = utf8_fromfd(fd_out);
    if (fout == NULL) {
        fprintf(stderr, "no memory\n");
        return 1;
    }

    for (;;) {
        uint32_t buf[512];
        int res = utf8_read(fin, buf, sizeof(buf) / sizeof(wchar_t));
        if (res == 0) {
            break;
        }
        if (res < 0) {
            fprintf(stderr, "utf8_read failed: %s", strerror(errno));
            return 1;
        }

        int count = res;
        uint32_t* curr = &buf[0];
        while (count > 0) {
            res = utf8_write(fout, curr, res);
            if (res <= 0) {
                fprintf(stderr, "utf8_write failed: %s", strerror(errno));
                return 1;
            }
            count -= res;
            curr += res;
        }
    }

    return 0;
}
