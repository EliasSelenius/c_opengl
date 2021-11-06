#include "fileIO.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>


char* fileread(const char* filename, u32* strLength) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("fileread() error");
    }

    fseek(file, 0, SEEK_END);
    *strLength = ftell(file);
    rewind(file);

    printf("file:%s has length: %d\n", filename, *strLength);

    char* res = malloc(sizeof(char) * (*strLength) + 1);
    fread(res, 1, *strLength, file);
    res[*strLength] = '\0';

    fclose(file);

    return res;
}

void filewrite(const char* filename, const char* text) {

}