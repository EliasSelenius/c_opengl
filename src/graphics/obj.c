#include "obj.h"
#include "stdio.h"
#include "stdlib.h"
#include "../math/vec.h"
#include "string.h"
#include "../String.h"
#include "../List.h"


void objFree(OBJfile* obj) {
    u32 len = listLength(obj->objs);

    for (u32 i = 0; i < len; i++) {
        free(obj->objs[i].name);
        listDelete(obj->objs[i].positions);
        listDelete(obj->objs[i].normals);
        listDelete(obj->objs[i].faces);
    }

    listDelete(obj->objs);
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
            out_data->vertices[vIndex].pos = obj->positions[pI];
            out_data->vertices[vIndex].normal = obj->normals[obj->faces[i].vertices[k].normal_index];
            out_data->vertices[vIndex].color = (vec4) { 1, 1, 1, 1 };

            vIndex++;
        }
    }
}

// smooth shading algo:    
void objToSmoothShadedMesh(OBJ* obj, MeshData* out_data) {
    out_data->vertexCount = listLength(obj->positions);
    out_data->vertices = malloc(sizeof(vertex) * out_data->vertexCount);
    out_data->indexCount = listLength(obj->faces) * 3;
    out_data->indices = malloc(sizeof(u32) * out_data->indexCount);

    for (int i = 0; i < out_data->vertexCount; i++) {
        out_data->vertices[i] = (vertex) {
            .pos = obj->positions[i],
            .normal = (vec3) {0,0,0},
            .color = (vec4) {1,1,1,1}
        };
    }

    u32 index = 0;
    for (int i = 0; i < listLength(obj->faces); i++) {
        // every vertex of one face has the same normal index, so its okay to do vertices[0]
        //u32 normalIndex = obj->faces[i].vertices[0].normal_index;

        // this is not the vertex normal, but it is the face normal
        //vec3 normal = obj->normals[normalIndex];

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


void objLoad(const char* filename, OBJfile* objFile) {
    FILE* file;

    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return;
    }

    objFile->objs = listCreate(OBJ);
    u32 objCount = 0;
    OBJ* obj = NULL;
    u32 basePosIndex = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;

        char* cursor = line;
        if ((cursor = stringStartsWith(line, "v "))) {


            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            vec3Add(&obj->pos, v);

            listAdd(obj->positions, v);

        } else if ((cursor = stringStartsWith(line, "vt "))) {

        } else if ((cursor = stringStartsWith(line, "vn "))) {
            
            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(obj->normals, v);

        } else if ((cursor = stringStartsWith(line, "f "))) {
            face tri;

            for (int i = 0; i < 3; i++) {
                
                tri.vertices[i].pos_index = strtol(cursor, &cursor, 10) - 1 - basePosIndex;
                cursor++;

                tri.vertices[i].uv_index = strtol(cursor, &cursor, 10) - 1;
                cursor++;

                tri.vertices[i].normal_index = strtol(cursor, &cursor, 10) - 1;
            }

            listAdd(obj->faces, tri);
            
        } else if ( (cursor = stringStartsWith(line, "o ")) ) {
            if (obj) {
                u32 len = listLength(obj->positions);
                // TODO: base index for uvs and normals
                basePosIndex += len;

                // calc object position
                vec3Scale(&obj->pos, 1.0f / (f32)len);
                // make vertex positions local
                for (u32 i = 0; i < len; i++) {
                    vec3Sub(&obj->positions[i], obj->pos);
                }
            }

            listAdd(objFile->objs, (OBJ){0});
            obj = &objFile->objs[objCount++];
            obj->positions = listCreate(vec3);
            obj->normals = listCreate(vec3);
            obj->faces = listCreate(face);

            u32 len = 0;
            while (*(cursor + ++len) != '\n');

            obj->name = malloc(len + 1);
            for (u32 i = 0; i < len; i++) {
                obj->name[i] = cursor[i];
            }
            obj->name[len] = '\0';
        }
    }

    fclose(file);


    { // print
        printf("%s\n", filename);
        u32 len = listLength(objFile->objs);
        for (u32 i = 0; i < len; i++) {
            printf("  %d. %s\n", i, objFile->objs[i].name);
        }

    }
}