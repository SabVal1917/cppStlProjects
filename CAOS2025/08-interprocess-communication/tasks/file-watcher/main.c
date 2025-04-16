#include <linux/limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Counter{
    char filename[PATH_MAX];
    int counter;
    struct Counter* next;
} Counter;

typedef struct Counters{
    struct Counter* head;
} Counters;

void increment(Counters* counters, char* filename, int value) {
    Counter* current = counters->head;
    while (current != NULL) {
        if (strncmp(current->filename, filename, PATH_MAX) == 0) {
            current->counter += value;
            return;
        }
        current = current->next;
    }
    Counter* new_head = malloc(sizeof(Counter));
    new_head->next = counters->head;
    new_head->counter = value;
    strncpy(new_head->filename, filename, PATH_MAX - 1);
    counters->head = new_head;
}

void print(Counters* counters) {
    Counter* current = counters->head;
    while (current != NULL) {
        printf("%s:%d\n", current->filename, current->counter);
        current = current->next;
    }
}

int main(int argc, char *argv[]) {
    Counters* counters = malloc(sizeof(Counter));
    counters->head = NULL;
    // your code goes here
    print(counters);
}
