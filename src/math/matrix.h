#pragma once

#include "../types.h"
#include "structs.h"


void mat4Mul(mat4* left, mat4* right, mat4* out_result);
void mat4MulScalar(mat4* m, float scalar);
void mat4MulVec3(vec3* res, mat4* m);


void mat4SetIdentity(mat4* m);
void mat4Transpose(mat4* m, mat4* out_result);


/*  NOTE: perspective functions derived from OpenTK
    
    fovy         :   Angle of the field of view in the y direction (in radians).
    aspect       :   Aspect ratio of the view (width / height).
    
    depthNear    :   Distance to the near clip plane.
    depthFar     :   Distance to the far clip plane.
    
    out_result   :   A projection matrix that transforms camera space to raster space.
*/
void mat4Perspective(f32 fovy, f32 aspect, f32 depthNear, f32 depthFar, mat4* out_result);
void mat4PerspectiveOffCenter(f32 left, f32 right, f32 bottom, f32 top, f32 depthNear, f32 depthFar, mat4* out_result);

void mat4Lookat(vec3* eye, vec3* target, vec3* up, mat4* out_result);

void pushMatrix();
void translate(vec3 t);
void scale(vec3 s);
void rotate(quat q);
void popMatrix(mat4* out_result);

