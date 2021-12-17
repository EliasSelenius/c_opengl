#pragma once

#include "types.h"
#include "math/vec.h"
#include "math/quat.h"

typedef struct Rigidbody {
    f32 mass;
    vec3 velocity;
    quat rotational_velocity;
} Rigidbody;

void rbAddForce(f32 fx, f32 fy, f32 fz);
void rbAddForceAtLocation(f32 fx, f32 fy, f32 fz, f32 px, f32 py, f32 pz);

void rbAddTorque(quat torque);