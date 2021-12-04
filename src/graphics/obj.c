#include "obj.h"
#include "stdio.h"
#include "stdlib.h"
#include "../math/vec.h"
#include "string.h"
#include "../String.h"
#include "../List.h"



void objLoad(const char* filename, MeshData* out_data) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Could not read file: %s\n", filename);
        perror("objLoad() error");
    }

    vec3* vertex_positions = listCreate(vec3);

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') continue;

        char* cursor = line;
        if ((cursor = stringStartsWith(cursor, "v "))) {

            vec3 v;
            v.x = strtof(cursor, &cursor);
            v.y = strtof(cursor, &cursor);
            v.z = strtof(cursor, &cursor);
            
            listAdd(vertex_positions, v);
        } /*else if ((cursor = stringStartsWith(cursor, "vt "))) {

        } else if ((cursor = stringStartsWith(cursor, "vn "))) {

        } else if ((cursor = stringStartsWith(cursor, "f "))) {

        }*/


        printf("%s", line);
    }

    fclose(file);


    for (int i = 0; i < listLength(vertex_positions); i++) {
        printf("[%i] = %f, %f, %f\n", i, vertex_positions[i].x, vertex_positions[i].y, vertex_positions[i].z);
    }

    listDelete(vertex_positions);
}