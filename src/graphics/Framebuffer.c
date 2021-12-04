#include "Framebuffer.h"

Framebuffer* framebufferCreate(u32 w, u32 h) {
    Framebuffer* res = malloc(sizeof(Framebuffer));
    res->width = w;
    res->height = h;

    return res;
}

void framebufferDelete(Framebuffer* framebuffer) {

}