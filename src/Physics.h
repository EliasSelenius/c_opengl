#pragma once

#include "types.h"
#include "math/vec.h"
#include "math/quat.h"
#include "math/Transform.h"

typedef struct Rigidbody {
    f32 mass;
    vec3 velocity;
    vec3 angularVelocity;
} Rigidbody;

void rbAddForce(Rigidbody* rb, vec3 force);
void rbAddForceAtLocation(Rigidbody* rb, vec3 force, vec3 offset);

void rbAddTorqueQuat(Rigidbody* rb, quat torque);
void rbAddTorque(Rigidbody* rb, vec3 axisAngle);


void rbUpdate(Rigidbody* rb, Transform* transform);

f32 raySphereDistance(vec3 spherePos, f32 sphereRadius, vec3 rayPos, vec3 rayDir);