#pragma once

#include "../types.h"
#include <GL.h>

typedef enum FramebufferDepthFormat {
    FBD_None = 0,
    FBD_DepthComponent = GL_DEPTH_COMPONENT

} FramebufferDepthFormat;

typedef enum FramebufferFormat {
    FBF_rgba8,
    FBF_rgb8,

    FBF_rgba16f,
    FBF_rgb16f,

    FBF_int32
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