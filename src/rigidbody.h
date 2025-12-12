#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "simple_logger.h"
#include "entity.h"

typedef struct Rigidbody{
//Everything we need to know- Position, velocity radius, mass, and bounciness
    float           mass_inverse;
    float           bounciness; 
    GFC_Vector3D    velocity;
    GFC_Vector3D    position;
    Entity          *owner;
    GFC_Sphere      *rigid_sphere;
    Uint8           onFloor;
    int             outOfBounds;
    float           friction;

}Rigidbody;

typedef struct PhysicsWorld {
    Rigidbody*  bodies;
    int         count;
    int         maxBodies;
} PhysicsWorld;

Entity *rigidbody_spawn(GFC_Vector3D position, GFC_Color color);
void rigidbody_on_collide(Entity *self, GFC_Vector3D normal);
#endif