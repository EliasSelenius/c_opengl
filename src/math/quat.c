#include "quat.h"
#include "matrix.h"
#include <math.h>


void quatMul(quat* left, quat* right, quat* out_result) {
    f32 a = left->w,
        b = left->x,
        c = left->y,
        d = left->z,

        e = right->w,
        f = right->x,
        g = right->y,
        h = right->z;

    *out_result = (quat){
        b*e + a*f + c*h - d*g,
        a*g - b*h + c*e + d*f,
        a*h + b*g - c*f + d*e, 
        a*e - b*f - c*g - d*h
    };
}

void quatNormalize(quat* q) {
    f32 l = sqrt(q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w);
    q->x /= l;
    q->y /= l;
    q->z /= l;
    q->w /= l;
}
void quatSetIdentity(quat* q) {
    *q = (quat){ 0, 0, 0, 1 };
}

void quatFromAxisAngle(vec3* axis, f32 angle, quat* out_result) {
    f32 ha = angle / 2.0f;
    f32 s = sin(ha);

    out_result->x = axis->x * s;
    out_result->y = axis->y * s;
    out_result->z = axis->z * s;
    out_result->w = cos(ha);
}

void quatToAxisAngle(quat* q, f32* out_angle, vec3* out_axis);

void quatFromMatrix(mat4* m, quat* out_result);
void quatToMatrix(quat* q, mat4* out_result) {
    f32 xx = q->x * q->x,
        xy = q->x * q->y,
        xz = q->x * q->z,
        xw = q->x * q->w,

        yy = q->y * q->y,
        yz = q->y * q->z,
        yw = q->y * q->w,

        zz = q->z * q->z,
        zw = q->z * q->w;

    *out_result = (mat4){
        1.0 - (yy + zz)*2.0,         (xy - zw)*2.0,         (xz + yw)*2.0,    0,
              (xy + zw)*2.0,   1.0 - (xx + zz)*2.0,         (yz - xw)*2.0,    0,
              (xz - yw)*2.0,         (yz + xw)*2.0,   1.0 - (xx + yy)*2.0,    0,
        0,                0,                     0,                           1
    };
}

void quatLeft(quat* q, vec3* v);
void quatUp(quat* q, vec3* v);
void quatForward(quat* q, vec3* v);
