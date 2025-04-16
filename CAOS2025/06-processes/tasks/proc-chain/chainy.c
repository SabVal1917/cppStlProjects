#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

enum { MAX_ARGS_COUNT = 256, MAX_CHAIN_LINKS_COUNT = 256 };

typedef struct {
  char* command;
  uint64_t argc;
  char* argv[MAX_ARGS_COUNT];
} chain_link_t;

typedef struct {
  uint64_t chain_links_count;
  chain_link_t chain_links[MAX_CHAIN_LINKS_COUNT];
} chain_t;

void create_chain(char* command_str, chain_t* chain) {
  char* copy_str = strdup(command_str);
  if (copy_str == NULL) {
    perror("strdup error");
    exit(EXIT_FAILURE);
  }
  chain->chain_links_count = 0;
  char* saveptr1 = NULL;
  char* saveptr2 = NULL;
  char* current_command = strtok_r(copy_str, "|", &saveptr1);
  while (current_command != NULL &&
         (chain->chain_links_count < MAX_CHAIN_LINKS_COUNT)) {
    // delete " " from begin and end, and nullterminate string
    while (*current_command == ' ') {
      current_command += 1;
    }
    char* end = current_command + strlen(current_command) - 1;
    while (current_command < end && *end == ' ') {
      *end = '\0';
      end -= 1;
    }
    // now we have good string, and we need to separate it by spaces
    chain_link_t* cur_chain_link =
        &chain->chain_links[chain->chain_links_count];
    cur_chain_link->argc = 0;
    char* cur_flag = strtok_r(current_command, " ", &saveptr2);
    while (cur_flag != NULL && cur_chain_link->argc < MAX_ARGS_COUNT) {
      if (cur_chain_link->argc == 0) {
        cur_chain_link->command = strdup(cur_flag);
        if (cur_chain_link->command == NULL) {
          perror("strdup error");
          exit(EXIT_FAILURE);
        }
        cur_chain_link->argv[cur_chain_link->argc++] = cur_chain_link->command;
      } else {
        cur_chain_link->argv[cur_chain_link->argc++] = strdup(cur_flag);
        if (cur_chain_link->argv[cur_chain_link->argc - 1] == NULL) {
          perror("strdup error");
          exit(EXIT_FAILURE);
        }
      }
      cur_flag = strtok_r(NULL, " ", &saveptr2);
    }
    cur_chain_link->argv[cur_chain_link->argc] =
        NULL;  // null terminate for execv
    current_command = strtok_r(NULL, "|", &saveptr1);
    chain->chain_links_count += 1;
  }
  free(copy_str);
}

void run_chain(chain_t* chain) {
  // we have chain of commands, and we need to get final result;
  int current_fd[2] = {-1, -1};
  int previous_fd[2] = {-1, -1};
  pid_t child_id[chain->chain_links_count];
  for (int i = 0; i < chain->chain_links_count; ++i) {
    //    need to open pipe
    if (pipe(current_fd) == -1) {
      perror("problem occurred with creating pipe\n");
      exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == -1) {
      perror("problem occurred with forking\n");
      exit(EXIT_FAILURE);
    }
    if (pid == 0) {
      if (i != 0) {
        if (dup2(previous_fd[0], STDIN_FILENO) == -1) {
          perror("problem occurred with stdin dup\n");
          exit(EXIT_FAILURE);
        }
      }
      if (i != chain->chain_links_count - 1) {
        if (dup2(current_fd[1], STDOUT_FILENO) == -1) {
          perror("problem occurred with stdout dup\n");
          exit(EXIT_FAILURE);
        }
      }
      close(previous_fd[0]);
      close(previous_fd[1]);
      close(current_fd[0]);
      close(current_fd[1]);
      execvp(chain->chain_links[i].command, chain->chain_links[i].argv);
      perror("problem occurred with execvp\n");
      exit(EXIT_FAILURE);
    } else {
      child_id[i] = pid;
    }
    if (previous_fd[0] != -1) {
      close(previous_fd[0]);
      close(previous_fd[1]);
    }
    previous_fd[0] = current_fd[0];
    previous_fd[1] = current_fd[1];
  }
  for (int i = 0; i < chain->chain_links_count; ++i) {
    if (waitpid(child_id[i], NULL, 0) == -1) {
      perror("problem occurred with waitpid\n");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }
  chain_t chain;
  create_chain(argv[1], &chain);
  run_chain(&chain);
  for (int i = 0; i < chain.chain_links_count; ++i) {
    free(chain.chain_links[i].command);
    for (int j = chain.chain_links[i].argc - 1; j > 0; --j) {
      free(chain.chain_links[i].argv[j]);
    }
  }
  return EXIT_SUCCESS;
}
