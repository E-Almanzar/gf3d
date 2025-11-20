#include "physicsworld.h"

PhysicsWorld gPhysicsWorld;

void physics_world_init(int maxBodies)
{
    gPhysicsWorld.bodies = calloc(maxBodies, sizeof(Rigidbody));
    gPhysicsWorld.count = 0;
    //gPhysicsWorld.maxBodies = maxBodies;
}

int physics_world_add(Rigidbody body)
{
    //if (gPhysicsWorld.count >= gPhysicsWorld.maxBodies)
    //    return -1;
    
    gPhysicsWorld.bodies[gPhysicsWorld.count] = body;
    return gPhysicsWorld.count++; // return ID
}