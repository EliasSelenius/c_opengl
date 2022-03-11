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
    // TODO: axis is angular acceleration, not torque?
    rbAddTorque(rb, axis);
}

void rbAddTorqueQuat(Rigidbody* rb, quat torque) {
    vec3 axis; f32 angle;
    quatToAxisAngle(&torque, &angle, &axis);
    vec3Scale(&axis, angle);
    rbAddTorque(rb, axis);
}

void rbAddTorque(Rigidbody* rb, vec3 axisAngle) {
    // TODO: mass / inertia ?
    vec3Add(&rb->angularVelocity, axisAngle);    
}

void rbUpdate(Rigidbody* rb, Transform* transform) {
    vec3 translation = rb->velocity;
    vec3Scale(&translation, app.deltatime);
    vec3Add(&transform->position, translation);
	
	
    vec3 axis = rb->angularVelocity;
    f32 len = vec3Length(axis);
    if (len > 0.0001) { // make sure the angularVelocity is not zero
        vec3Scale(&axis, 1.0f / len); // normalize axis
        transformRotateAxisAngle(transform, axis, len * app.deltatime);
    }
}