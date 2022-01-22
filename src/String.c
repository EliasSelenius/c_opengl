#include "String.h"
#include "List.h"

#include <string.h>
#include <stdlib.h>

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
    sb->capacity = 16;
    sb->length = 0;
    sb->content = malloc(sb->capacity);
}

void sbDestroy(StringBuilder* sb) {
    free(sb->content);
    sb->content = NULL;
    sb->capacity = sb->length = 0;
}

static void growBufferSize(StringBuilder* sb, u32 additionalSpace) {
    u32 newLength = sb->length + additionalSpace;

    // NOTE: we do <= (as opposed to only <) to make sure there is space for zero termination.
    if (sb->capacity <= newLength) {
        sb->capacity *= 2;

        // make sure there is sufficent storage space
        while (sb->capacity <= newLength)
            sb->capacity *= 2;
        
        sb->content = realloc(sb->content, sb->capacity);
    }
}

void sbAppend(StringBuilder* sb, char* str) {
    u32 strLen = strlen(str);
    growBufferSize(sb, strLen);

    u32 i = 0;
    while (true) {
        if (str[i] == '\0') break;
        sb->content[sb->length++] = str[i];
        i++;
    }

    // make sure the content is zero-terminated, so that it can be used as a c string
    sb->content[sb->length] = '\0';
}

void sbAppendView(StringBuilder* sb, Strview str) {
    growBufferSize(sb, str.length);

    for (int i = 0; i < str.length; i++) {
        sb->content[sb->length++] = str.data[i];
    }

    // make sure the content is zero-terminated, so that it can be used as a c string
    sb->content[sb->length] = '\0';
}

void sbCopyIntoBuffer(StringBuilder* sb, char* buffer, u32 bufferLength) {
    for (int i = 0; i < sb->length && i < bufferLength; i++) {
        buffer[i] = sb->content[i];
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