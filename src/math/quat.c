#include "quat.h"
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

quat conjugate(quat q) {
    q.x = -q.x;
    q.y = -q.y;
    q.z = -q.z;
    return q;
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

void quatSlerp(quat* a, quat* b, f32 t, quat* out_result) {
    // v0 and v1 should be unit length or else
    // something broken will happen.
    
    // Compute the cosine of the angle between the two vectors.
    f64 dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;
    
    const f64 DOT_THRESHOLD = 0.9995;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
    
        *out_result = (quat) {
            a->x + t * (b->x - a->x), 
            a->y + t * (b->y - a->y), 
            a->z + t * (b->z - a->z), 
            a->w + t * (b->w - a->w)
        };

        quatNormalize(out_result);
        return;
    }
    
    
    // Clamp(dot, -1, 1);           // Robustness: Stay within domain of acos()
    f32 theta_0 = acos(dot);     // theta_0 = angle between input vectors
    f32 theta = theta_0 * t;     // theta = angle between v0 and result 
    
    // Quaternion v2 = v1 – v0*dot;
    // v2.normalize();              // { v0, v2 } is now an orthonormal basis
    
    // return v0*cos(theta) + v2*sin(theta);
    
    quat c = (quat) {
        b->x - a->x * dot,
        b->y - a->y * dot,
        b->z - a->z * dot,
        b->w - a->w * dot
    };
    quatNormalize(&c);

    f32 cos_t = cos(theta);
    f32 sin_t = sin(theta);

    *out_result = (quat) {
        a->x * cos_t + c.x * sin_t,
        a->y * cos_t + c.y * sin_t,
        a->z * cos_t + c.z * sin_t,
        a->w * cos_t + c.w * sin_t,
        
    };
}

void quatRotateVec3(quat rot, vec3* v) {

    quat p = (quat) { v->x, v->y, v->z, 0 }; // construct a quaternion whose i j k coefficients are x y z respectively, and real part is equal zero

    quatMul(&rot, &p, &p);

    // create the conjugate of rot
    rot.x = -rot.x;
    rot.y = -rot.y;
    rot.z = -rot.z;

    quatMul(&p, &rot, &p);

    *v = p.xyz;
}

void quatFromAxisAngle(vec3* axis, f32 angle, quat* out_result) {
    f32 ha = angle / 2.0f;
    f32 s = sin(ha);
    
    out_result->x = axis->x * s;
    out_result->y = axis->y * s;
    out_result->z = axis->z * s;
    out_result->w = cos(ha);
}

void quatToAxisAngle(quat* q, f32* out_angle, vec3* out_axis) {
    if (q->w > 1.0f) quatNormalize(q);
    f32 _acos = acos(q->w);
    *out_angle = 2.0f * _acos;
    f32 _sin = sin(_acos);
    if (_sin < 0.001f) *out_axis = (vec3) { 1, 0, 0 };
    else *out_axis = (vec3) { q->x / _sin, q->y / _sin, q->z / _sin };
}

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
