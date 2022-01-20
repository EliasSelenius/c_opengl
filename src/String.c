#include "String.h"
#include <string.h>
#include "List.h"

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

Strview svTrimChar(Strview sv, char c) {
    return svTrimCharStart(svTrimCharEnd(sv, c), c);
}

Strview svTrimCharStart(Strview sv, char c) {
    while (*sv.data == c) {
        sv.data++;
        sv.length--;
    }
    return sv;
}

Strview svTrimCharEnd(Strview sv, char c) {
    char* end = sv.data + sv.length;
    while ((*--end) == c);
    sv.length = end - sv.data + 1;
    return sv;
}


b8 svStartsWith(Strview sv, char* start) {
    u32 i = 0;
    while (true) {
        if (start[i] == '\0') return true;
        if (sv.data[i] == '\0') return false;
        if (start[i] != sv.data[i]) return false;
        i++;
    }
}

void sbInit(StringBuilder* sb) {
    sb->length = 0;
    sb->sections = listCreate(Strview);    
}

void sbDestroy(StringBuilder* sb) {
    listDelete(sb->sections);
}

void sbAppend(StringBuilder* sb, Strview str) {
    sb->length += str.length;
    listAdd(sb->sections, str);
}

void sbCopyIntoBuffer(StringBuilder* sb, char* buffer, u32 bufferLength) {
    u32 sectionsCount = listLength(sb->sections);
    u32 index = 0;
    for (u32 i = 0; i < sectionsCount; i++) {
        Strview sv = sb->sections[i];
        for (u32 j = 0; j < sv.length; j++) {
            if (index >= bufferLength) return;
            buffer[index] = sv.data[j];
            index++;
        }
    }
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