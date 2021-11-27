#pragma once

#include "../types.h"

typedef enum FramebufferFormat {
    FBF_rgba8,
    FBF_rgb8,

    FBF_rgba16f,
    FBF_rgb16f,

    FBF_int32
} FramebufferFormat;

typedef struct Framebuffer {
    u32 id;
    u32 width, height;

    u32 attachmentCount;
    FramebufferFormat* attachments;

} Framebuffer;

Framebuffer* framebufferCreate(u32 w, u32 h);
void framebufferDelete(Framebuffer* framebuffer);