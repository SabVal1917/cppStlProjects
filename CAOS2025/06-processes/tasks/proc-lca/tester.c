#include <assert.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lca.h"

void test_same() {
    pid_t pid = getpid();

    pid_t expected_result = pid;
    pid_t result = find_lca(pid, pid);

    assert(result == expected_result && "Wrong LCA for the same node!");
}

void test_parent() {
    pid_t pid = getpid();
    pid_t ppid = getppid();

    pid_t expected_result = ppid;
    pid_t result = find_lca(pid, ppid);

    assert(result == expected_result && "Wrong LCA for parent and child!");
}

void test_siblings() {
    pid_t first_child = fork();
    if (first_child > 0) {
        pid_t second_child = fork();
        if (second_child > 0) {
            pid_t expected_result = getpid();
            pid_t result = find_lca(first_child, second_child);

            wait(NULL);
            wait(NULL);

            assert(result == expected_result && "Wrong LCA for siblings!");

            return;

        } else if (second_child < 0) perror("test_siblings:fork");

    } else if (first_child < 0) perror("test_siblings:fork");

    exit(0);
}

void forkforkfork(pid_t* some_pid) {
    pid_t root = getpid();

    for (int i = 0; i < rand() % 4; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            fork(); fork();
            *some_pid = getpid();
            break;
        }
    }

    while (true) {
        sched_yield();
    }
}

void subtest_random() {
    pid_t expected_result = getpid();

    pid_t* shared_pids = (pid_t*)mmap(NULL, 16, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_pids == NULL) perror("test_random:mmap");

    shared_pids[0] = 1;
    shared_pids[1] = 1;

    pid_t to_terminate[2];

    // Left
    if (getpid() == expected_result) {
        pid_t pid = fork();
        if (pid == 0) {
            setpgrp();
            forkforkfork(&shared_pids[0]);
        }
        to_terminate[0] = pid;
    }

    // Right
    if (getpid() == expected_result) {
        pid_t pid = fork();
        if (pid == 0) {
            setpgrp();
            forkforkfork(&shared_pids[1]);
        };
        to_terminate[1] = pid;
    }

    if (getpid() == expected_result) {
        sleep(1);

        pid_t result = find_lca(shared_pids[0], shared_pids[1]);

        kill(-to_terminate[0], SIGTERM);
        kill(-to_terminate[1], SIGTERM);

        assert(result == expected_result && "Wrong LCA!");

        int munmap_result = munmap(shared_pids, 16);
        if (munmap_result < 0) perror("test_random:munmap");
    }
}

void test_random() {
    for (uint64_t seed = 0; seed < 4; ++seed) {
        srand(seed);
        subtest_random();
    }
}

int main() {
    test_same();
    test_parent();
    test_siblings();

    test_random();

    return 0;
}
