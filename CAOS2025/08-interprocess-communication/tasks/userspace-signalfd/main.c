#include <assert.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "signalfd.h"

int main(int argc, char* argv[]) {
    int my_pid = getpid();
    int signal_fd = signalfd();
    for (int sig_num = 1; sig_num < 32; ++sig_num) {
        if ((sig_num != SIGKILL) && (sig_num != SIGSTOP)) {
            kill(my_pid, sig_num);
            int real_sig_num;
            assert(read(signal_fd, &real_sig_num, sizeof(int)) == sizeof(int));
            assert(sig_num == real_sig_num);
        }
    }
    return 0;
}
