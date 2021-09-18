
#include "entity.h"
#include <stdlib.h>


entity* create_entity(Transform* transform, const u32 count, ...) {
    entity* ent = malloc(sizeof(entity));
    ent->transform = *transform;
    ent->count = count;
    ent->comps = malloc(sizeof(component) * count);

    va_list pargs;
    va_start(pargs, count);

    for (u32 i = 0; i < count; i++) {
        component c;
        c.update = va_arg(pargs, updateFunc);
        c.data = va_arg(pargs, void*);
        c.datasize = 0;
        ent->comps[i] = c;
    }

    va_end(pargs);

    return ent;
}

void update_entity(entity* ent) {
    for (u32 i = 0; i < ent->count; i++) {
        ent->comps[i].update(&ent->comps[i]);
    }
}