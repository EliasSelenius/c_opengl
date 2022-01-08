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

    

	{ // make offset be local to rb rotation
	
		mat4 m;
		transformToMatrix(rb->transform, &m);
	
		offset = (vec3) {
			offset.x * m.row1.x   +   offset.y * m.row2.x   +   offset.z * m.row3.x,
			offset.x * m.row1.y   +   offset.y * m.row2.y   +   offset.z * m.row3.y,
			offset.x * m.row1.z   +   offset.y * m.row2.z   +   offset.z * m.row3.z
		};	
	}


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

void rbUpdate(Rigidbody* rb) {
    vec3 translation = rb->velocity;
    vec3Scale(&translation, app.deltatime);
    vec3Add(&rb->transform->position, translation);


    // printf("deltaT: %f\n", app.deltatime);


    // rotate and scale by deltatime
    quat rotation;
    quatMul(&rb->transform->rotation, &rb->rotational_velocity, &rotation);   // transforms rotation after one second
    quatNormalize(&rotation);

    quatSlerp(&rb->transform->rotation, &rotation, app.deltatime, &rotation); // scale by deltatime

    quatNormalize(&rotation);
    rb->transform->rotation = rotation; 
}