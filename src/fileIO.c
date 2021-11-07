#include "fileIO.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

char* fileread(const char* filename, u32* strLength) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("fileread() error");
    }

    fseek(file, 0, SEEK_END);
    *strLength = ftell(file);
    rewind(file);


    char* res = calloc(*strLength + 1, sizeof(char));
    fread(res, 1, *strLength, file);

    //printf("file:%s has length: %d, and strlen = %llu\n", filename, *strLength, strlen(res));

    fclose(file);

    return res;
}

void filewrite(const char* filename, const char* text) {

}