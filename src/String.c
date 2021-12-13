#include "String.h"
#include <string.h>

String* stringCreate(const char* text) {
    int len = strlen(text);
    String* res = malloc(sizeof(String) + sizeof(char) * len);
    res->length = len;
    for (int i = 0; i < len; i++) { res->characters[i] = text[i]; }

    return res;
}

char* stringStartsWith(char* text, const char* start) {
    int i = 0;
    while (true) {
        if (start[i] == '\0') return &text[i];
        if (text[i] == '\0') return NULL;
        if (text[i] != start[i]) return NULL;
        i++;
    }
}

void stringTrimEnd(char* str, char c) {
    char* end = str + strlen(str);
    while (*--end == c);
    *(end+1) = '\0';  
    
}