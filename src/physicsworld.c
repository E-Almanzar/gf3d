#include "physicsworld.h"

PhysicsWorld gPhysicsWorld;

void physics_world_init(int maxBodies)
{
    gPhysicsWorld.bodies = calloc(maxBodies, sizeof(Rigidbody));
    gPhysicsWorld.count = 0;
    gPhysicsWorld.maxBodies = maxBodies;
    //slog("WHAT %i", gPhysicsWorld.maxBodies);
}

int physics_world_add(Rigidbody body)
{
    //slog("We have %i physicsbodies: %i %i ", gPhysicsWorld.count, gPhysicsWorld.count,gPhysicsWorld.maxBodies);
    if (gPhysicsWorld.count >= gPhysicsWorld.maxBodies){
        return -1;
    }

    gPhysicsWorld.bodies[gPhysicsWorld.count] = body;
    return gPhysicsWorld.count++; // return ID
}

void circle_circle_check(Rigidbody *firstBody){
    int i, didCollide = 0;
    float dist, squaredDist = 0, radii;
    Rigidbody *secondBody;
    GFC_Vector3D difference, normal;
    normal.x = 0; normal.y = 0; normal.z = 0;
    for(i = 0; i < gPhysicsWorld.maxBodies; i++){
        secondBody = &gPhysicsWorld.bodies[i];
        if(!secondBody){return;} 

        //TODO fix this? it segfaults if theres a continue 
        if( secondBody == firstBody){return;}

        didCollide = gfc_sphere_overlap(*firstBody->rigid_sphere, *secondBody->rigid_sphere);
        if(didCollide){
            //slog("ooh we hit it");
                        
            /*Vector3 diff = bodyA->position - bodyB->position;
            float dist = length(diff);
            float r = A->radius + B->radius;

            if(dist < r) {
                float penetration = r - dist;
                Vector3 normal = diff / dist;
            }*/
            difference.x = firstBody->rigid_sphere->x - secondBody->rigid_sphere->x;
            difference.y = firstBody->rigid_sphere->y - secondBody->rigid_sphere->y;
            difference.z = firstBody->rigid_sphere->z - secondBody->rigid_sphere->z;
            squaredDist = difference.x*difference.x+difference.y*difference.y+difference.z*difference.z;
            //Get the squared radius
            radii = firstBody->rigid_sphere->r + secondBody->rigid_sphere->r;
            radii *= radii;

            if(squaredDist < radii){
                dist = sqrt(squaredDist);
                //slog("Difference: %f,%f,%f",difference.x,difference.y,difference.z);
                //slog("Dist: %f, = %f",dist, difference.x/dist);

                normal.x = difference.x/dist;
                normal.y = difference.y/dist;
                normal.z = difference.z/dist;
                //slog("In  circle: %f, %f, %f ", normal.x, normal.y, normal.z);
            }
            //DO both of their collides
            rigidbody_on_collide(firstBody->owner, normal);
            rigidbody_on_collide(secondBody->owner, normal);
        }
        //else{slog("no we didnt");}
    }
}

void physics_step(){
     for (int i = 0; i <= gPhysicsWorld.count; i++) {
        Rigidbody *rb = &gPhysicsWorld.bodies[i];
        //slog("%f, %f, %f", rb->velocity.x, rb->velocity.y, rb->velocity.z);
                if(!rb->owner){
            //slog("no owner %i", i);
            return;
        }
        //slog("B4??");
        rb->velocity.x = rb->owner->velocity.x;
        rb->velocity.y = rb->owner->velocity.y;
        rb->velocity.z = rb->owner->velocity.z;
        //slog("after?");
        rb->position.x += rb->velocity.x;
        rb->position.y += rb->velocity.y;
        rb->position.z += rb->velocity.z;

        if(!rb->owner){
            //slog("no owner %i", i);
            return;
        }
        
        rb->rigid_sphere->x = rb->position.x;
        rb->rigid_sphere->y = rb->position.y;
        rb->rigid_sphere->z = rb->position.z;

        if(rb->owner->data){
            circle_circle_check(rb);
        }

        gfc_vector3d_copy(rb->owner->position, rb->position);
    }


    //Sync back
    
}