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

    if (obj->child) objFree(obj->child);
    if (obj->next) objFree(obj->next);

    free(obj);
}


    // TODO: make adaptive algo by doing smooth shading or flat shading based on angle between normals 


// basic flat shading algo:
void objToFlatShadedMesh(OBJ* obj, MeshData* out_data) {
    u32 len = out_data->vertexCount = out_data->indexCount = listLength(obj->faces) * 3;
    out_data->vertices = malloc(sizeof(vertex) * len);
    out_data->indices = malloc(sizeof(u32) * len);
    out_data->groups = null;
    
    u32 vIndex = 0;
    for (int i = 0; i < listLength(obj->faces); i++) {
        for (int k = 0; k < 3; k++) {
            // indecies:
            out_data->indices[vIndex] = vIndex;

            // vertex:
            u32 pI = obj->faces[i].vertices[k].pos_index;
            out_data->vertices[vIndex].pos = obj->vertex_positions[pI];
            out_data->vertices[vIndex].normal = obj->vertex_normals[obj->faces[i].vertices[k].normal_index];

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
    out_data->groups = obj->groups;
    out_data->materialLibrary = obj->mtllib;

    for (int i = 0; i < out_data->vertexCount; i++) {
        out_data->vertices[i] = (vertex) {
            .pos = obj->vertex_positions[i],
            .normal = (vec3) {0,0,0}
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


static MTL* mtlLoad(const char* filename) {
    FILE* file;
    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return null;
    }

    MTL* firstMtl = null;
    MTL* mtl = null;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* cursor = line;
        while (*cursor++ != ' ');

        switch (line[0]) {

            case '#': continue;

            // newmtl
            case 'n': {
                MTL* newmtl = malloc(sizeof(MTL));
                newmtl->name = getStringUntilNewline(cursor);
                newmtl->next = null;

                if (mtl) mtl->next = newmtl;
                else firstMtl = newmtl;

                mtl = newmtl;

            } break;

            case 'K': {
                vec3 value = (vec3) { strtof(cursor, &cursor), strtof(cursor, &cursor), strtof(cursor, &cursor) };
                switch (line[1]) {
                    case 'a': mtl->Ka = value; break;
                    case 'd': mtl->Kd = value; break;
                    case 's': mtl->Ks = value; break;
                    case 'e': mtl->Ke = value; break;
                }
            } break;

        }
    }

    fclose(file);

    return firstMtl;
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
    obj->position = center;
    u32 len = listLength(obj->vertex_positions);
    for (u32 i = 0; i < len; i++) {
        vec3Sub(&obj->vertex_positions[i], center);
    }
}

static OBJ* parseOBJ(FILE* file, char* objname, u32* basePosIndex) {
    OBJ* obj = malloc(sizeof(OBJ));
    obj->mtllib = null;
    obj->child = null;
    obj->name = objname;
    obj->next = null;
    obj->position = (vec3){0};
    obj->boundingbox[0] = (vec3){0};
    obj->boundingbox[1] = (vec3){0};

    obj->vertex_positions = listCreate(vec3);
    obj->vertex_normals = listCreate(vec3);
    obj->faces = listCreate(face);
    obj->groups = listCreate(VertexGroup);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // char* cursor = &line[2];
        char* cursor = line;
        while (*cursor++ != ' ');

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

            case 'u': {
                // usemtl
                // NOTE: we are not checking if the token is 'usemtl'. This is probably not nescesary.

                VertexGroup g;
                g.materialName = getStringUntilNewline(cursor);
                g.start = listLength(obj->faces) * 3;
                g.count = -g.start; // NOTE: clever trick

                listAdd(obj->groups, g);

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
    
    { // calc vertex group count
        u32 groupCount = listLength(obj->groups);
        if (groupCount) {
            groupCount -= 1;
            for (u32 i = 0; i < groupCount; i++) {
                obj->groups[i].count += obj->groups[i + 1].start;
            }
            obj->groups[groupCount].count = listLength(obj->faces) * 3 - obj->groups[groupCount].start;
        }
    }

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
    printf("%s -> (groups: %d)\n", obj->name, listLength(obj->groups));

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

    c->position = v3sub(c->position, p->position);

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

void objGetMesh(OBJ* obj, Mesh* mesh) {
    MeshData data;
    objToSmoothShadedMesh(obj, &data);
    meshFromData(&data, mesh);

    free(data.vertices);
    free(data.indices);
}

OBJ* objGetByIndex(OBJ* obj, u32 index) {
    while (index) {
        obj = obj->next;
        index--;
    }
    return obj;
}
u32 objChildCount(OBJ* obj) {
    u32 i = 0;
    obj = obj->child;
    while (obj) {
        i++;
        obj = obj->next;
    }
    return i;
}

static u32 lastIndexOf(const char* string, char c) {
    u32 res = 0;
    u32 i = 0;
    while (string[i] != '\0') {
        if (string[i] == c) res = i;
        i++;
    }
    return res;
}

OBJ* objLoad(const char* filename) {
    FILE* file;

    if (fopen_s(&file, filename, "r")) {
        printf("Could not read file: %s\n", filename);
        return NULL;
    }

    u32 basePosIndex = 0;
    OBJ* res;
    char* mtllib_name = null;


    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;
        if (line[0] == 'o') {
            res = parseOBJ(file, getStringUntilNewline(&line[2]), &basePosIndex);
            break;
        }

        // mtllib
        if (line[0] == 'm') {
            mtllib_name = getStringUntilNewline(&line[7]);
        }
    }


    fclose(file);

    if (mtllib_name) {

        u32 lastI = lastIndexOf(filename, '/');
        char mtlfile[64];
        u32 i = 0;
        for (; i <= lastI; i++) mtlfile[i] = filename[i];

        u32 len = strlen(mtllib_name);
        for (u32 j = 0; j < len; j++) mtlfile[i++] = mtllib_name[j];
        mtlfile[i] = '\0';


        res->mtllib = mtlLoad(mtlfile);

        OBJ* next = res->next;
        while (next) {
            next->mtllib = res->mtllib;
            next = next->next;
        }

        free(mtllib_name);
    }

    // { // print
    //     printf("%s\n", filename);
        
    //     OBJ* cobj = res;
    //     u32 i = 0;
    //     while (cobj) {
    //         printf("  %d. %s\n", i++, cobj->name);
    //         cobj = cobj->next;
    //     }
    // }

    { // create node graph
        OBJ* parent = res;
        OBJ* current = parent->next;

        while (current) {
            OBJ* next = current->next;

            if (AABBintersects(parent->boundingbox, current->boundingbox)) {
                current->next = null;
                addChild(parent, current);
                parent->next = null;
            } else {
                parent->next = current;
                parent = current;
            }

            current = next;
        }
    }

    { // print node graph
        // printf("NODE GRAPH: %s\n", filename);
        // printOBJ(res, 0);        
    }

    { // print material lib
        // printf("Materials: (%s)\n", filename);
        // MTL* mtl = res->mtllib;
        // while (mtl) {
        //     printf("  %s\n", mtl->name);
        //     mtl = mtl->next;
        // }
    }

    return res;
}