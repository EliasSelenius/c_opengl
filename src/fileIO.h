#pragma once
#include "types.h"


char* fileread(const char* filename, u32* strLength);
void filewrite(const char* filename, const char* text);