#include "matrix.h"
#include "vec.h"

void mat4Mul(mat4* left, mat4* right, mat4* out_result) {
    vec4 c1 = { right->row1.x, right->row2.x, right->row3.x, right->row4.x };
    vec4 c2 = { right->row1.y, right->row2.y, right->row3.y, right->row4.y };
    vec4 c3 = { right->row1.z, right->row2.z, right->row3.z, right->row4.z };
    vec4 c4 = { right->row1.w, right->row2.w, right->row3.w, right->row4.w };

    mat4 out;

    out.row1.x = vec4dot(&left->row1, &c1);
    out.row1.y = vec4dot(&left->row1, &c2);
    out.row1.z = vec4dot(&left->row1, &c3);
    out.row1.w = vec4dot(&left->row1, &c4);

    out.row2.x = vec4dot(&left->row2, &c1);
    out.row2.y = vec4dot(&left->row2, &c2);
    out.row2.z = vec4dot(&left->row2, &c3);
    out.row2.w = vec4dot(&left->row2, &c4);

    out.row3.x = vec4dot(&left->row3, &c1);
    out.row3.y = vec4dot(&left->row3, &c2);
    out.row3.z = vec4dot(&left->row3, &c3);
    out.row3.w = vec4dot(&left->row3, &c4);

    out.row4.x = vec4dot(&left->row4, &c1);
    out.row4.y = vec4dot(&left->row4, &c2);
    out.row4.z = vec4dot(&left->row4, &c3);
    out.row4.w = vec4dot(&left->row4, &c4);

    *out_result = out;
}

void mat4SetIdentity(mat4* m) {
    m->row1 = (vec4){ 1, 0, 0, 0 };
    m->row2 = (vec4){ 0, 1, 0, 0 };
    m->row3 = (vec4){ 0, 0, 1, 0 };
    m->row4 = (vec4){ 0, 0, 0, 1 };
}

void mat4Perspective(f32 fovy, f32 aspect, f32 depthNear, f32 depthFar, mat4* out_result) {
    f32 maxY = depthNear * tan(0.5f * fovy);
    f32 minY = -maxY;
    f32 minX = minY * aspect;
    f32 maxX = maxY * aspect;

    mat4PerspectiveOffCenter(minX, maxX, minY, maxY, depthNear, depthFar, out_result);
}

void mat4PerspectiveOffCenter(f32 left, f32 right, f32 bottom, f32 top, f32 depthNear, f32 depthFar, mat4* out_result) {
    f32 x = 2.0f * depthNear / (right - left);
    f32 y = 2.0f * depthNear / (top - bottom);
    f32 a = (right + left) / (right - left);
    f32 b = (top + bottom) / (top - bottom);
    f32 c = -(depthFar + depthNear) / (depthFar - depthNear);
    f32 d = -(2.0f * depthFar * depthNear) / (depthFar - depthNear);

    out_result->row1 = (vec4){ x,  0,  0,  0 };
    out_result->row2 = (vec4){ 0,  y,  0,  0 };
    out_result->row3 = (vec4){ a,  b,  c, -1 };
    out_result->row4 = (vec4){ 0,  0,  d,  0 };
}
