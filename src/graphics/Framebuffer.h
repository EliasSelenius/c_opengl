#pragma once

#include "../types.h"
#include <GL.h>

typedef enum FramebufferDepthFormat {
    fbDepth_None = 0,
    fbDepth_DepthComponent = GL_DEPTH_COMPONENT,
    fbDepth_DepthComponentTexture

} FramebufferDepthFormat;

typedef enum FramebufferFormat {
    fbFormat_rgba8,
    fbFormat_rgb8,

    fbFormat_rgba16f,
    fbFormat_rgb16f,

    fbFormat_int32,

    fbFormat_float16,
    fbFormat_float32
    
} FramebufferFormat;

typedef struct FBattachment {
    u32 texture;
    FramebufferFormat format;
} FBattachment;

typedef struct Framebuffer {
    u32 id;
    u32 width, height;

    u32 attachmentCount;
    FBattachment* attachments;

    FramebufferDepthFormat depthFormat;
    u32 depthAttachment;

} Framebuffer;


Framebuffer* framebufferCreate(u32 w, u32 h, u32 attachmentCount, FramebufferFormat* attachmentFormats, FramebufferDepthFormat depthFormat);
void framebufferDelete(Framebuffer* framebuffer);
void framebufferResize(Framebuffer* framebuffer, u32 w, u32 h);