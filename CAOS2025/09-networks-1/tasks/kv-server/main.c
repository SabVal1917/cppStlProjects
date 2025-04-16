#include <linux/limits.h>
#include <stdlib.h>
#include <string.h>

typedef struct StorageItem{
    char key[PATH_MAX];
    char value[PATH_MAX];
    struct StorageItem* next;
} StorageItem;

typedef struct Storage{
    struct StorageItem* head;
} Storage;

StorageItem* find(Storage* storage, char* key) {
    StorageItem* current = storage->head;
    while (current != NULL) {
        if (strncmp(current->key, key, PATH_MAX) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void set(Storage* storage, char* key, char* value) {
    StorageItem* element = find(storage, key);
    if (element == NULL) {
        element = malloc(sizeof(StorageItem));
        strncpy(element->key, key, PATH_MAX - 1);
        element->next = storage->head;
        storage->head = element;
    }
    strncpy(element->value, value, PATH_MAX - 1);
}

char* get(Storage* storage, char* key) {
    StorageItem* element = find(storage, key);
    if (element == NULL) {
        return "";
    } else {
        return element->value;
    }
}

// your code goes here

int main(int argc, char* argv[])
{
    Storage* storage = malloc(sizeof(Storage));
    storage->head = NULL;
    int server_port = atoi(argv[1]);
    // your code goes here
    return 0;
}
