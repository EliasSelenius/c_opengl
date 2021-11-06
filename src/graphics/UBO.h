#pragma once

#include "../types.h"

typedef struct {
    char* name;
    u32 bindingPoint;

    u32 bufferId;
} Ublock;

Ublock* ublockGetByName(char* name);

void ublockBindBuffer(Ublock* ublock, u32 buffer);

void ublockPrintAll();