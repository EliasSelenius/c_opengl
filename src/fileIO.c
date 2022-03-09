#include "fileIO.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

b8 fileExists(const char* filename) {
    if (filename == NULL) return false;
    
    struct stat filestat;
    
    return stat(filename, &filestat) == 0;
}


char* fileread(const char* filename, u32* strLength) {
    FILE* file;
    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *strLength = ftell(file);
    rewind(file);

    char* res = calloc(*strLength + 1, sizeof(char));
    fread(res, 1, *strLength, file);

    fclose(file);

    return res;
}

void filewrite(const char* filename, const char* text) {

}