#pragma once

#include "types.h"
#include "math/vec.h"
#include "math/quat.h"
#include "math/Transform.h"

typedef struct Rigidbody {
    f32 mass;
    vec3 velocity;
    quat rotational_velocity;
} Rigidbody;

void rbAddForce(Rigidbody* rb, vec3 force);
void rbAddForceAtLocation(Rigidbody* rb, vec3 force, vec3 offset);

void rbAddTorque(Rigidbody* rb, quat torque);

void rbApplyToTransform(Rigidbody* rb, Transform* t);