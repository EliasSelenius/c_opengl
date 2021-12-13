#pragma once

#include "types.h"

typedef struct {
    u32 length;
    char characters[];
} String;

String* stringCreate(const char* text);

char* stringStartsWith(char* text, const char* start);

void stringTrimEnd(char* str, char c);