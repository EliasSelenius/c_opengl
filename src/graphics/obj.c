#include "obj.h"
#include "stdio.h"
#include "stdlib.h"
#include "../math/vec.h"
#include "string.h"
#include "../String.h"
#include "../List.h"


void objFree(OBJ* obj) {
    listDelete(obj->positions);
    listDelete(obj->normals);
    listDelete(obj->faces);
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


/*

    algo draft:

        create lookup table of verts with size of listLength(obj->positions)

        loop faces
            loop verts in face
                check to see if the vertex already exist in lookup table
                if so:
                    test face normal with every other face that is registered to this vertex
                    if the face normal deviates 
                if not:
                    initialize the vertex in lookup table
                    add to final vertex array
        

*/

typedef struct Node {
    face* face;
    struct Node* next;
} Node;

static void addFace(Node* node, face* face) {
    if (node->face) {
        node->next = malloc(sizeof(Node));
        node->next->face = face;
    } else {
        node->face = face;
    }
}

void objToMesh(OBJ* obj, u32 normal_threshold_angle, MeshData* out_data) {
    
    u32 vertFacesLen = listLength(obj->positions);
    Node vertFaces[vertFacesLen];
    
    for (int i = 0; i < listLength(obj->faces); i++) {
        face* face = &obj->faces[i];
        for (int k = 0; k < 3; k++) {
            addFace(&vertFaces[face->vertices[k].pos_index], face);
        }
    
    
        u32 normalIndex = face->vertices[0].normal_index;
        vec3 faceNormal = obj->normals[normalIndex];


    }

    for (int i = 0; i < vertFacesLen; i++) {
        Node* node = &vertFaces[i];

    }
}


void objLoad(const char* filename, OBJ* out_obj) {
    FILE* file;

    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return;
    }

    out_obj->positions = listCreate(vec3);
    out_obj->normals = listCreate(vec3);
    out_obj->faces = listCreate(face);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;

        char* cursor = line;
        if ((cursor = stringStartsWith(line, "v "))) {
            
            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(out_obj->positions, v);

        } else if ((cursor = stringStartsWith(line, "vt "))) {

        } else if ((cursor = stringStartsWith(line, "vn "))) {
            
            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(out_obj->normals, v);

        } else if ((cursor = stringStartsWith(line, "f "))) {
            face tri;

            for (int i = 0; i < 3; i++) {
                
                tri.vertices[i].pos_index = strtol(cursor, &cursor, 10) - 1;
                cursor++;

                tri.vertices[i].uv_index = strtol(cursor, &cursor, 10) - 1;
                cursor++;

                tri.vertices[i].normal_index = strtol(cursor, &cursor, 10) - 1;
            }

            listAdd(out_obj->faces, tri);
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

}