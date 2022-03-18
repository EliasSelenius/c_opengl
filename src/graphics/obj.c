#include "obj.h"
#include "stdio.h"
#include "stdlib.h"
#include "../math/vec.h"
#include "string.h"
#include "../String.h"
#include "../List.h"
#include "../math/general.h"

void objFree(OBJ* obj) {

    free(obj->name);
    listDelete(obj->vertex_positions);
    listDelete(obj->vertex_normals);
    listDelete(obj->faces);

    if (obj->next) objFree(obj->next);

    free(obj);
}


    // TODO: make adaptive algo by doing smooth shading or flat shading based on angle between normals 


// basic flat shading algo:
void objToFlatShadedMesh(OBJ* obj, MeshData* out_data) {
    u32 len = out_data->vertexCount = out_data->indexCount = listLength(obj->faces) * 3;
    out_data->vertices = malloc(sizeof(vertex) * len);
    out_data->indices = malloc(sizeof(u32) * len);
    u32 vIndex = 0;
    for (int i = 0; i < listLength(obj->faces); i++) {
        for (int k = 0; k < 3; k++) {
            // indecies:
            out_data->indices[vIndex] = vIndex;

            // vertex:
            u32 pI = obj->faces[i].vertices[k].pos_index;
            out_data->vertices[vIndex].pos = obj->vertex_positions[pI];
            out_data->vertices[vIndex].normal = obj->vertex_normals[obj->faces[i].vertices[k].normal_index];
            out_data->vertices[vIndex].color = (vec4) { 1, 1, 1, 1 };

            vIndex++;
        }
    }
}

// smooth shading algo:    
void objToSmoothShadedMesh(OBJ* obj, MeshData* out_data) {
    out_data->vertexCount = listLength(obj->vertex_positions);
    out_data->vertices = malloc(sizeof(vertex) * out_data->vertexCount);
    out_data->indexCount = listLength(obj->faces) * 3;
    out_data->indices = malloc(sizeof(u32) * out_data->indexCount);

    for (int i = 0; i < out_data->vertexCount; i++) {
        out_data->vertices[i] = (vertex) {
            .pos = obj->vertex_positions[i],
            .normal = (vec3) {0,0,0},
            .color = (vec4) {1,1,1,1}
        };
    }

    u32 index = 0;
    for (int i = 0; i < listLength(obj->faces); i++) {
        // every vertex of one face has the same normal index, so its okay to do vertices[0]
        //u32 normalIndex = obj->faces[i].vertices[0].normal_index;

        // this is not the vertex normal, but it is the face normal
        //vec3 normal = obj->vertex_normals[normalIndex];

        for (int k = 0; k < 3; k++) {
            u32 vIndex = obj->faces[i].vertices[k].pos_index;
            
            //vec3* np = &out_data->vertices[vIndex].normal;
            //vec3Add(np, normal); // add face normal to vertex normal
            
            out_data->indices[index++] = vIndex;
        }
    }

    /*
    for (int i = 0; i < out_data->vertexCount; i++) {
        vec3Normalize(&out_data->vertices[i].normal);
    }
    */

    meshGenNormals(out_data);   

}


void objToMesh(OBJ* obj, u32 normal_threshold_angle, MeshData* out_data) {

}

typedef struct MTL {
    char* name;
    
} MTL;

static char* getStringUntilNewline(char* start) {
    u32 len = 0;
    while (*(start + ++len) != '\n');

    char* str = malloc(len + 1);
    for (u32 i = 0; i < len; i++) {
        str[i] = start[i];
    }
    str[len] = '\0';
    return str;
}

void mtlLoad(const char* filename) {
    FILE* file;
    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* cursor = line;
        if ( (cursor = stringStartsWith(line, "newmtl ")) ) {

            
        }
    }

    fclose(file);
}

// TODO: probably should make this a mesh thing
static void approximateCenterOfMass(OBJ* obj) {
    vec3 center = {0};
    u32 facesLen = listLength(obj->faces);
    f32 div = 0;
    for (u32 i = 0; i < facesLen; i++) {
        face f = obj->faces[i];
        vec3 v1 = obj->vertex_positions[f.vertices[0].pos_index];
        vec3 v2 = obj->vertex_positions[f.vertices[1].pos_index];
        vec3 v3 = obj->vertex_positions[f.vertices[2].pos_index];

        f32 w = v3length(cross(v3sub(v2, v1), v3sub(v3, v1)));
        vec3 triCenter = v3scale(v3add(v1, v3add(v2, v3)), 1.0f / 3.0f);
        w = w * w * w;
        div += w;
        vec3Add(&center, v3scale(triCenter, w));
    }

    center = v3scale(center, 1.0f / div);
    u32 len = listLength(obj->vertex_positions);
    for (u32 i = 0; i < len; i++) {
        vec3Sub(&obj->vertex_positions[i], center);
    }
}

static OBJ* parseOBJ(FILE* file, char* objname, u32* basePosIndex) {
    OBJ* obj = malloc(sizeof(OBJ));
    obj->child = null;
    obj->name = objname;
    obj->next = NULL;
    obj->position = (vec3){0};
    obj->boundingbox[0] = (vec3){0};
    obj->boundingbox[1] = (vec3){0};

    obj->vertex_positions = listCreate(vec3);
    obj->vertex_normals = listCreate(vec3);
    obj->faces = listCreate(face);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* cursor = &line[2];
        switch (line[0]) {
            
            // comment
            case '#': continue;

            // vertex attributes
            case 'v': { // vertex
                switch (line[1]) {
                    
                    case ' ': { // position
                        vec3 v = { strtof(cursor, &cursor), strtof(cursor, &cursor), strtof(cursor, &cursor) };
                        vec3Add(&obj->position, v);
                        listAdd(obj->vertex_positions, v);
                    } break;


                    case 't': { // texture coord

                    } break;


                    case 'n': { // normal
                        vec3 v = { strtof(cursor, &cursor), strtof(cursor, &cursor), strtof(cursor, &cursor) };            
                        listAdd(obj->vertex_normals, v);
                    } break;

                    default: 
                        // error
                    break;
                }
            } break;

            // face (triangle)
            case 'f': {
                face tri;
                for (int i = 0; i < 3; i++) {
                    tri.vertices[i].pos_index = strtol(cursor, &cursor, 10) - 1 - *basePosIndex;
                    cursor++; // jump over '/'
                    tri.vertices[i].uv_index = strtol(cursor, &cursor, 10) - 1;
                    cursor++; // jump over '/'
                    tri.vertices[i].normal_index = strtol(cursor, &cursor, 10) - 1;
                }

                listAdd(obj->faces, tri);
            } break;

            // object
            case 'o': {
                *basePosIndex += listLength(obj->vertex_positions);
                obj->next = parseOBJ(file, getStringUntilNewline(cursor), basePosIndex);
                goto finalizeObject;
            } break;
        }            
    }

finalizeObject:
    
    u32 vlen = listLength(obj->vertex_positions);
    
    // calc average vertex position
    vec3Scale(&obj->position, 1.0f / (f32)vlen);

    // calc bounding box
    obj->boundingbox[0] = obj->position;
    obj->boundingbox[1] = obj->position;
    for (u32 i = 0; i < vlen; i++) {
        vec3 v = obj->vertex_positions[i];
        if (v.x < obj->boundingbox[0].x) obj->boundingbox[0].x = v.x;
        if (v.y < obj->boundingbox[0].y) obj->boundingbox[0].y = v.y;
        if (v.z < obj->boundingbox[0].z) obj->boundingbox[0].z = v.z;
        if (v.x > obj->boundingbox[1].x) obj->boundingbox[1].x = v.x;
        if (v.y > obj->boundingbox[1].y) obj->boundingbox[1].y = v.y;
        if (v.z > obj->boundingbox[1].z) obj->boundingbox[1].z = v.z;
    }

    approximateCenterOfMass(obj);

    return obj;
}

static void printOBJ(OBJ* obj, u32 level) {
    if (obj == null) return;

    for (u32 i = 0; i < level; i++) printf("  ");
    printf("%s\n", obj->name);

    printOBJ(obj->child, level + 1);
    printOBJ(obj->next, level);
}

static u32 intersects(f32 min1, f32 max1, f32 min2, f32 max2) {
    return !((max1 < min2) || (max2 < min1));
}

u32 AABBintersects(vec3 aabb1[2], vec3 aabb2[2]) {
    return intersects(aabb1[0].x, aabb1[1].x, aabb2[0].x, aabb2[1].x)
        && intersects(aabb1[0].y, aabb1[1].y, aabb2[0].y, aabb2[1].y)
        && intersects(aabb1[0].z, aabb1[1].z, aabb2[0].z, aabb2[1].z);
}

static void addChild(OBJ* p, OBJ* c) {
    if (p->child) {
        p = p->child;
        while (p->next) {
            p = p->next;
        }
        p->next = c;
    } else {
        p->child = c;
    }
}

OBJ* objLoad(const char* filename) {
    FILE* file;

    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return NULL;
    }

    u32 basePosIndex = 0;
    OBJ* res;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;
        if (line[0] == 'o') {
            res = parseOBJ(file, getStringUntilNewline(&line[2]), &basePosIndex);
            break;
        }
    }


    fclose(file);

    { // print
        printf("%s\n", filename);
        
        OBJ* cobj = res;
        u32 i = 0;
        while (cobj) {
            printf("  %d. %s\n", i++, cobj->name);
            cobj = cobj->next;
        }
    }

    { // create node graph
        OBJ* parent = res;
        OBJ* current = parent->next;

        while (current) {
            OBJ* next = current->next;

            if (AABBintersects(parent->boundingbox, current->boundingbox)) {
                current->next = null;
                addChild(parent, current);
            } else {
                parent->next = current;
                parent = current;
            }

            current = next;
        }
    }

    { // print node graph
        printf("NODE GRAPH: %s\n", filename);
        printOBJ(res, 0);        
    }

    return res;
}