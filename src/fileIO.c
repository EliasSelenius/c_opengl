#include "fileIO.h"

#include <stdio.h>

char* fileread(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        perror("fileread() error");
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    rewind(file);

    char* res = malloc(sizeof(char) * length + 1);
    fread(res, 1, length, file);
    res[length] = (char)0;


    fclose(file);

    return res;
}

void filewrite(const char* filename, const char* text) {

}