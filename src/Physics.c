#include "Physics.h"
#include "Application.h"
#include "math/quat.h"
#include "math/vec.h"
#include "types.h"

#include <stdio.h>

void rbAddForce(Rigidbody* rb, vec3 force) {
    rb->velocity.x += force.x / rb->mass;
    rb->velocity.y += force.y / rb->mass;
    rb->velocity.z += force.z / rb->mass;
}

void rbAddForceAtLocation(Rigidbody* rb, vec3 force, vec3 offset) {
    rbAddForce(rb, force);

    vec3 axis;
    vec3Cross(&force, &offset, &axis);
    
    f32 len = vec3Length(axis);
    vec3Scale(&axis, 1.0f / len); // normalize axis

    // len = vec3Length(offset);
    quat torque;
    quatFromAxisAngle(&axis, len, &torque);
    rbAddTorque(rb, torque);
}

void rbAddTorque(Rigidbody* rb, quat torque) {
    quatMul(&rb->rotational_velocity, &torque, &rb->rotational_velocity);
    // quatNormalize(&rb->rotational_velocity);
}

void rbApplyToTransform(Rigidbody* rb, Transform* t) {
    vec3 translation = rb->velocity;
    vec3Scale(&translation, app.deltatime);
    vec3Add(&t->position, translation);


    quat rotation = rb->rotational_velocity;
    vec3 axis; f32 angle;
    quatToAxisAngle(&rotation, &angle, &axis);
    angle *= app.deltatime;
    quatFromAxisAngle(&axis, angle, &rotation);
    quatNormalize(&rotation);
    transformRotate(t, rotation);
}