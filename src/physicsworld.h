#ifndef __PHYSICSWORLD_H__
#define __PHYSICSWORLD_H__

#include "rigidbody.h"

void physics_world_init(int maxBodies);
int physics_world_add(Rigidbody body);
void physics_step();
void body_apply_gravity(Rigidbody *rb);
void body_validate_position(Rigidbody *rb);
Uint8 circle_ground_check(Rigidbody *rb);
void physics_world_close();
#endif