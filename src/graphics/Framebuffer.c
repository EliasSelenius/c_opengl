#include "Framebuffer.h"
#include "glUtils.h"
#include <GL.h>

#include <stdlib.h>
#include <stdio.h>

static void getGLFormat(FramebufferFormat format, GLint* out_internalFormat, GLenum* out_format, GLenum* out_type) {
    switch (format) {
        case fbFormat_rgba8:
            *out_internalFormat = GL_RGBA8;
            *out_format         = GL_RGBA;
            *out_type           = GL_BYTE;
            break;
        case fbFormat_rgb8: 
            *out_internalFormat = GL_RGB8;
            *out_format         = GL_RGB;
            *out_type           = GL_BYTE;
            break;


        case fbFormat_rgba16f: 
            *out_internalFormat = GL_RGBA16F;
            *out_format         = GL_RGBA;
            *out_type           = GL_FLOAT;
            break;
        case fbFormat_rgb16f:
            *out_internalFormat = GL_RGB16F;
            *out_format         = GL_RGB;
            *out_type           = GL_FLOAT;
            break;


        case fbFormat_int32:
            *out_internalFormat = GL_R32I;
            *out_format         = GL_RED_INTEGER;
            *out_type           = GL_INT;
            break;

        case fbFormat_float16:
            *out_internalFormat = GL_R16F;
            *out_format         = GL_RED;
            *out_type           = GL_FLOAT;
            break;
        case fbFormat_float32:
            *out_internalFormat = GL_R32F;
            *out_format         = GL_RED;
            *out_type           = GL_FLOAT;
            break;

        default:
            printf("Framebuffer format not supported\n");
    }
}


static void initDepth(Framebuffer* fb) {
    if (fb->depthFormat == fbDepth_DepthComponentTexture) {
        glBindTexture(GL_TEXTURE_2D, fb->depthAttachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fb->width, fb->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        glBindRenderbuffer(GL_RENDERBUFFER, fb->depthAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, fb->depthFormat, fb->width, fb->height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

Framebuffer* framebufferCreate(u32 w, u32 h, u32 attachmentCount, FramebufferFormat* attachmentFormats, FramebufferDepthFormat depthFormat) {
    Framebuffer* res = malloc(sizeof(Framebuffer));
    res->width = w;
    res->height = h;

    glGenFramebuffers(1, &res->id);
    glBindFramebuffer(GL_FRAMEBUFFER, res->id);


    res->depthFormat = depthFormat;
    if (res->depthFormat != fbDepth_None) {
        if (res->depthFormat == fbDepth_DepthComponentTexture) {
            glGenTextures(1, &res->depthAttachment);
            glBindTexture(GL_TEXTURE_2D, res->depthAttachment);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode_ClampToEdge);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode_ClampToEdge);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Nearest);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Nearest);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, res->width, res->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            
            glBindTexture(GL_TEXTURE_2D, 0);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, res->depthAttachment, 0);
        } else {
            glGenRenderbuffers(1, &res->depthAttachment);
            initDepth(res);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, res->depthAttachment);
        }
    }


    res->attachmentCount = attachmentCount;
    res->attachments = malloc(sizeof(FBattachment) * attachmentCount);

    GLenum bufs[attachmentCount];
    for (u32 i = 0; i < attachmentCount; i++) {
        bufs[i] = GL_COLOR_ATTACHMENT0 + i;

        GLint internalFormat;
        GLenum format, type;
        getGLFormat(attachmentFormats[i], &internalFormat, &format, &type);

        u32 t = texture2DCreate(w, h, false, Filter_Nearest, WrapMode_ClampToEdge, internalFormat, format, type);

        res->attachments[i] = (FBattachment) {
            .format = attachmentFormats[i],
            .texture = t
        };

        glFramebufferTexture2D(GL_FRAMEBUFFER, bufs[i], GL_TEXTURE_2D, t, 0);
    }
    glDrawBuffers(attachmentCount, bufs);


    GLenum code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (code != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer error code: %d\n", code);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return res;
}

void framebufferDelete(Framebuffer* framebuffer) {

}

void framebufferResize(Framebuffer* framebuffer, u32 w, u32 h) {
    framebuffer->width = w;
    framebuffer->height = h;

    if (framebuffer->depthFormat != fbDepth_None) {
        initDepth(framebuffer);
    }

    for (u32 i = 0; i < framebuffer->attachmentCount; i++) {
        FBattachment attach = framebuffer->attachments[i];
        GLint internalFormat;
        GLenum format, type;
        getGLFormat(attach.format, &internalFormat, &format, &type);
        glBindTexture(GL_TEXTURE_2D, attach.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}