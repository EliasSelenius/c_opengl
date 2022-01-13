#pragma once

#include "types.h"
#include "math/vec.h"
#include "math/quat.h"
#include "math/Transform.h"

typedef struct Rigidbody {
    f32 mass;
    vec3 velocity;
    vec3 angularVelocity;
    Transform* transform;
} Rigidbody;

void rbAddForce(Rigidbody* rb, vec3 force);
void rbAddForceAtLocation(Rigidbody* rb, vec3 force, vec3 offset);

void rbAddTorqueQuat(Rigidbody* rb, quat torque);
void rbAddTorque(Rigidbody* rb, vec3 axisAngle);


void rbUpdate(Rigidbody* rb);