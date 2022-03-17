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


OBJ* objLoad(const char* filename) {
    FILE* file;

    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return NULL;
    }

    OBJ* firstObj = NULL;
    u32 objCount = 0;
    OBJ* obj = NULL;
    u32 basePosIndex = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;

        char* cursor = line;
        if ((cursor = stringStartsWith(line, "v "))) {
            // vertex position
            vec3 v = { strtof(cursor, &cursor), strtof(cursor, &cursor), strtof(cursor, &cursor) };
            vec3Add(&obj->position, v);
            listAdd(obj->vertex_positions, v);

        } else if ((cursor = stringStartsWith(line, "vt "))) {
            // vertex uv
        } else if ((cursor = stringStartsWith(line, "vn "))) {
            // vertex normal
            vec3 v = { strtof(cursor, &cursor), strtof(cursor, &cursor), strtof(cursor, &cursor) };            
            listAdd(obj->vertex_normals, v);

        } else if ((cursor = stringStartsWith(line, "f "))) {
            face tri;
            for (int i = 0; i < 3; i++) {
                tri.vertices[i].pos_index = strtol(cursor, &cursor, 10) - 1 - basePosIndex;
                cursor++; // jump over '/'
                tri.vertices[i].uv_index = strtol(cursor, &cursor, 10) - 1;
                cursor++; // jump over '/'
                tri.vertices[i].normal_index = strtol(cursor, &cursor, 10) - 1;
            }

            listAdd(obj->faces, tri);
            
        } else if ( (cursor = stringStartsWith(line, "o ")) ) {
            // object
            OBJ* newobj = malloc(sizeof(OBJ));
            newobj->next = NULL;
            newobj->position = (vec3){0};

            if (obj) {
                obj->next = newobj;

                u32 len = listLength(obj->vertex_positions);
                // TODO: base index for uvs and normals
                basePosIndex += len;

                // calc averageVertexPos
                vec3Scale(&obj->position, 1.0f / (f32)len);
                
                // make vertex positions local
                // for (u32 i = 0; i < len; i++) {
                //     vec3Sub(&obj->vertex_positions[i], obj->pos);
                // }           

                vec3 center = {0};
                u32 facesLen = listLength(obj->faces);
                for (u32 i = 0; i < facesLen; i++) {
                    face f = obj->faces[i];
                    vec3 v1 = obj->vertex_positions[f.vertices[0].pos_index];
                    vec3 v2 = obj->vertex_positions[f.vertices[1].pos_index];
                    vec3 v3 = obj->vertex_positions[f.vertices[2].pos_index];

                    vec3 triCenter = v3scale(v3add(v1, v3add(v2, v3)), 1.0f / 3.0f);
                    vec3Add(&center, triCenter);
                }

                center = v3scale(center, 1.0f / (f32)facesLen);
                for (u32 i = 0; i < len; i++) {
                    vec3Sub(&obj->vertex_positions[i], center);
                }

                center = (vec3){0};
                f32 div = 0;
                for (u32 i = 0; i < facesLen; i++) {
                    face f = obj->faces[i];
                    vec3 v1 = obj->vertex_positions[f.vertices[0].pos_index];
                    vec3 v2 = obj->vertex_positions[f.vertices[1].pos_index];
                    vec3 v3 = obj->vertex_positions[f.vertices[2].pos_index];

                    f32 w = v3length(cross(v3sub(v2, v1), v3sub(v3, v1)));
                    vec3 triCenter = v3scale(v3add(v1, v3add(v2, v3)), 1.0f / 3.0f);
                    // w *= 1000.0f;
                    w = w * w * w * w * w;
                    div += w;
                    vec3Add(&center, v3scale(triCenter, w));
                }

                center = v3scale(center, 1.0f / div);
                for (u32 i = 0; i < len; i++) {
                    vec3Sub(&obj->vertex_positions[i], center);
                }

            } else {
                firstObj = newobj;
            }

            obj = newobj;
            obj->vertex_positions = listCreate(vec3);
            obj->vertex_normals = listCreate(vec3);
            obj->faces = listCreate(face);

            obj->name = getStringUntilNewline(cursor);

        } else if ( (cursor = stringStartsWith(line, "usemtl ")) ) {
            
        }
    }

    fclose(file);


    { // print
        printf("%s\n", filename);
        
        OBJ* cobj = firstObj;
        u32 i = 0;
        while (cobj) {
            printf("  %d. %s\n", i++, cobj->name);
            cobj = cobj->next;
        }
    }

    return firstObj;
}