#ifndef __PHYSICSWORLD_H__
#define __PHYSICSWORLD_H__

#include "rigidbody.h"

void physics_world_init(int maxBodies);
int physics_world_add(Rigidbody body);

#endif