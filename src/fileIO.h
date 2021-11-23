#pragma once
#include "types.h"

b8 fileExists(const char* filename);
char* fileread(const char* filename, u32* strLength);
void filewrite(const char* filename, const char* text);