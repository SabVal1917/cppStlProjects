#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct signalfd_state {
  int pipefd[2];
  sigset_t mask;
};

static struct signalfd_state state;

static void handle_signal(int signum) {
  if (write(state.pipefd[1], &signum, sizeof(signum)) != sizeof(signum)) {
    perror("write");
    return;
  }
}

int signalfd() {
  pipe(state.pipefd);
  sigemptyset(&state.mask);
  for (int sig_num = 1; sig_num < 32; ++sig_num) {
    if (sig_num != SIGKILL && sig_num != SIGSTOP) {
      if (signal(sig_num, handle_signal) == SIG_ERR) {
        perror("signal");
        exit(1);
      }
    }
  }
  return state.pipefd[0];
}
