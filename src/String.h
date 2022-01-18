#pragma once

#include "types.h"

typedef struct {
    u32 length;
    char* data;
} Strview;


Strview svFrom(char* str);
void svCopyTo(Strview src, char* dest);
Strview svTrim(Strview sv);
Strview svTrimStart(Strview sv);
Strview svTrimEnd(Strview sv);





char* stringStartsWith(char* text, const char* start);

void stringTrimEnd(char* str, char c);