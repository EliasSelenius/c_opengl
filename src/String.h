#pragma once

#include "prelude.h"

typedef struct Strview {
    u32 length;
    char* data;
} Strview;

Strview svFrom(char* str);
void svCopyTo(Strview src, char* dest);

Strview svTrim(Strview sv);
Strview svTrimStart(Strview sv);
Strview svTrimEnd(Strview sv);

Strview svTrimChar(Strview sv, char c);
Strview svTrimCharStart(Strview sv, char c);
Strview svTrimCharEnd(Strview sv, char c);

b8 svStartsWith(Strview sv, char* start);


// TODO: make StringBuilder just like lists
// replace *content with content[]
// replace void sbInit(StringBuilder*) with StringBuilder* sbCreate()
typedef struct StringBuilder {
    u32 length;
    u32 capacity;
    char* content;
} StringBuilder;

void sbInit(StringBuilder* sb);
void sbDestroy(StringBuilder* sb);
void sbAppend(StringBuilder* sb, char* str);
void sbAppendView(StringBuilder* sb, Strview str);
void sbCopyIntoBuffer(StringBuilder* sb, char* buffer, u32 bufferLength);




char* stringStartsWith(char* text, const char* start);

void stringTrimEnd(char* str, char c);

bool spanEquals(Strview span, char* str);
bool spanEqualsSpan(Strview span1, Strview span2);

inline bool stringEquals(char* a, char* b) {
    u32 i = 0;
    while (true) {
        if (a[i] != b[i]) return false;
        if (a[i] == '\0') return true;
        i++;
    }
}