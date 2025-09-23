#include "io.h"

int fileExists(char *path) {
    return access(path, F_OK);
}

FILE* readFile(char *path) {    
    if (fileExists(path) < 0) {
        printf("File unavailable in path %s\n", path);
        return NULL;
    }

    return fopen(path, "rb");
}