#include "String.h"
#include <string.h>


Strview svFrom(char* str) {
    return (Strview) {
        .length = strlen(str),
        .data = str
    };
}

void svCopyTo(Strview src, char* dest) {
    for (u32 i = 0; i < src.length; i++) dest[i] = src.data[i];
    // TODO: should we null terminate dest?
}

Strview svTrim(Strview sv) {
    return svTrimStart(svTrimEnd(sv));
}

static b8 isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

Strview svTrimStart(Strview sv) {
    while (isWhitespace(*sv.data)) {
        sv.data++;
        sv.length--;
    }
    return sv;
}

Strview svTrimEnd(Strview sv) {
    char* end = sv.data + sv.length;
    while (isWhitespace(*--end));
    sv.length = end - sv.data + 1;
    return sv;
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