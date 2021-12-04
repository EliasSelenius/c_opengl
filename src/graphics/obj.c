#include "obj.h"
#include "stdio.h"
#include "stdlib.h"
#include "../math/vec.h"
#include "string.h"
#include "../String.h"
#include "../List.h"

typedef struct {
    u32 pos_index, normal_index, uv_index;
} vertex_index;

typedef struct {
    vertex_index vertices[3];
} face;

void objLoad(const char* filename, MeshData* out_data) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Could not read file: %s\n", filename);
        perror("objLoad() error");
    }

    vec3* positions = listCreate(vec3);
    vec3* normals = listCreate(vec3);
    face* faces = listCreate(face);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;

        char* cursor = line;
        if ((cursor = stringStartsWith(line, "v "))) {
            
            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(positions, v);

        } else if ((cursor = stringStartsWith(line, "vt "))) {

        } else if ((cursor = stringStartsWith(line, "vn "))) {
            
            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(normals, v);

        } else if ((cursor = stringStartsWith(line, "f "))) {
            face tri;

            for (int i = 0; i < 3; i++) {
                
                tri.vertices[i].pos_index = strtol(cursor, &cursor, 10) - 1;
                cursor++;

                tri.vertices[i].uv_index = strtol(cursor, &cursor, 10) - 1;
                cursor++;

                tri.vertices[i].normal_index = strtol(cursor, &cursor, 10) - 1;
            }

            listAdd(faces, tri);
        }


        //printf("%s", line);
    }

    fclose(file);


    /*for (int i = 0; i < listLength(positions); i++) {
        printf("[%i] = %f, %f, %f\n", i, positions[i].x, positions[i].y, positions[i].z);
    }
    for (int i = 0; i < listLength(normals); i++) {
        printf("[%i] = %f, %f, %f\n", i, normals[i].x, normals[i].y, normals[i].z);
    }

    for (int i = 0; i < listLength(faces); i++) {
        printf("[%i] = ", i);
        for (int j = 0; j < 3; j++) {
            printf("%i, %i, %i      ", faces[i].vertices[j].pos_index, faces[i].vertices[j].uv_index, faces[i].vertices[j].normal_index);
        }
        printf("\n");
    }*/

    // TODO: make smooth shading by combining vertices into one, based on angle between normals 
    // basic flat shading algo:
    u32 len = out_data->vertexCount = out_data->indexCount = listLength(faces) * 3;
    out_data->vertices = malloc(sizeof(vertex) * len);
    out_data->indices = malloc(sizeof(u32) * len);
    for (int i = 0; i < len; i++) {
        out_data->indices[i] = i;
        out_data->vertices[i].pos = faces[i / 3].vertices[]
    }


    listDelete(positions);
    listDelete(normals);
    listDelete(faces);
}