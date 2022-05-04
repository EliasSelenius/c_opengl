#include "prelude.h"
#include "math/Transform.h"
#include "graphics/Mesh.h"

// typedef struct Material {

// } Material;

typedef struct Model {
    Transform transform;
    u32 vertices_count, indices_count;
    vertex* vertices;
    u32* indices;
    u32 parent_index;
} Model;

/*
    Package data layout
        header - struct Package
        bytes  - the raw data from disk
            - list of models
            - list of vertices
            - list of indices
*/
typedef struct Package {
    Model* models;
    u32 models_count;

    vertex* vertices;
    u32* indices;

    char raw_data[];
} Package;


void packageCreate() {

}

Package* packageLoad() {
    return null;
}