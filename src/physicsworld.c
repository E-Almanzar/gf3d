#include "physicsworld.h"
#include "monster_thinks.h"
#include "evilball.h"
#define GRAVITY -.4
#define MU .9
PhysicsWorld gPhysicsWorld;
void hit_wall(Rigidbody *rb);

void set_position_to_origin(Rigidbody *rb){
    rb->position.x = 0;
    rb->position.y = 0; 
    rb->position.z = 0;
}

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
    gPhysicsWorld.bodies[gPhysicsWorld.count]._inuse = true;
    return gPhysicsWorld.count++; // return ID
}

Uint8 circle_circle_check(Rigidbody *firstBody, Rigidbody *secondBody, GFC_Vector3D *normal){
    int i, didCollide = 0;
    float dist, squaredDist = 0, radii;
    //Rigidbody *secondBody;
    GFC_Vector3D difference;//, normal;
    normal->x = 0; normal->y = 0; normal->z = 0;
    //We should make it only do it for these two
    //for(i = 0; i < gPhysicsWorld.maxBodies; i++){
    //    secondBody = &gPhysicsWorld.bodies[i];
    if(!secondBody){return 0;} 

    //TODO fix this? it segfaults if theres a continue - we should not continue
    if( secondBody == firstBody){return 0;}

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
        difference.x = secondBody->rigid_sphere->x - firstBody->rigid_sphere->x;
        difference.y = secondBody->rigid_sphere->y - firstBody->rigid_sphere->y;
        difference.z = secondBody->rigid_sphere->z - firstBody->rigid_sphere->z;
        squaredDist = difference.x*difference.x+difference.y*difference.y+difference.z*difference.z;
        //Get the squared radius
        radii = firstBody->rigid_sphere->r + secondBody->rigid_sphere->r;
        radii *= radii;

        if(squaredDist < radii){
            dist = sqrt(squaredDist);
            //slog("Difference: %f,%f,%f",difference.x,difference.y,difference.z);
            //slog("Dist: %f, = %f",dist, difference.x/dist);

            normal->x = difference.x/dist;
            normal->y = difference.y/dist;
            normal->z = difference.z/dist;
            //slog("In  circle: %f, %f, %f ", normal.x, normal.y, normal.z);
        }
        //DO both of their collides
        //slog("we hit?");
        //gfc_vector3d_negate(*normal, *normal);
        //normal->x *=-1; normal->y *=-1; normal->z *=-1;
        rigidbody_on_collide(firstBody->owner, secondBody->owner, *normal);
        rigidbody_on_collide(secondBody->owner, firstBody->owner, *normal);
        /*
        if (gfc_stricmp("Ball", firstBody->owner->name) == 0){
            rigidbody_on_collide(firstBody->owner, secondBody->owner, *normal);
            rigidbody_on_collide(secondBody->owner, firstBody->owner, *normal);
        }
        else if(gfc_stricmp("Evil Ball", firstBody->owner->name) == 0 || gfc_stricmp("Evil Ball", secondBody->owner->name)){
            //rigidbody_on_collide(firstBody->owner,  firstBody->owner, *normal);
            evilball_on_collide(secondBody->owner, *normal);
            evilball_on_collide(firstBody->owner, *normal);
            slog("we hit? %s %s", firstBody->owner->name, secondBody->owner->name);

            //0 means do nothing, 1 means we collidied and would like to multiply on firstbody
        }*/
    }
    return 0;
        //else{slog("no we didnt");}
    //}
}


//Theres a big problem- everyone checks velocites, then checks collisions, then moves! bad
//Is that even an issue? It checks, then looks for collisions, 
void physics_step(){
    Rigidbody *A, *B, *rb;
    Uint8 weCollided = 0;
    float velNormal, impulseScalar, e;
    GFC_Vector3D relativeVelocity, normal, impulse, temp, distance;
     for (int i = 0; i < gPhysicsWorld.count; i++) {
        rb = &gPhysicsWorld.bodies[i];
        //slog("%f, %f, %f", rb->velocity.x, rb->velocity.y, rb->velocity.z);
        if(!rb->owner){
            //slog("no owner %i", i);
            return;
        }

        //slog("B4??");

        rb->velocity.x = rb->velocity.x < .000001 ? 0 : rb->velocity.x;
        rb->velocity.y = rb->velocity.y < .000001 ? 0 : rb->velocity.y;

        rb->velocity.x = rb->owner->velocity.x;
        rb->velocity.y = rb->owner->velocity.y;
        rb->velocity.z = rb->owner->velocity.z;
        //Apply Friction
        if(!rb->friction){
            rb->velocity.x *= MU;
            rb->velocity.y *= MU;
        }
        else{
            rb->velocity.x *= rb->friction;
            rb->velocity.y *= rb->friction;
        }/*
        rb->velocity.x = rb->velocity.x < .000001 ? 0 : rb->velocity.x;
        rb->velocity.y = rb->velocity.y < .000001 ? 0 : rb->velocity.y;
        */
        //slog("after?");
        
        //APPLY GRAVITY TO BODIES AT THE START OF EACH STEP?
        //GRAVITY OFF
        //body_apply_gravity(rb);
        
        //We want to only apply the positions if theyre valid?
        //body_validate_position(rb);

        //Move the actual positions?

        rb->position.x += rb->velocity.x;
        rb->position.y += rb->velocity.y;
        if((gfc_stricmp("Alien Guy", rb->owner->name) != 0)){
            if(rb->owner->think == bounce_think){
                rb->position.z += rb->velocity.z;

            }
            else {
                body_apply_gravity(rb);
            }
        }
        //rb->position.z += rb->velocity.z;
        //The Z will be handled by apply
        //body_apply_gravity(rb);
        if(!rb->owner){
            //slog("no owner %i", i);
            return;
        }

        rb->rigid_sphere->x = rb->position.x;
        rb->rigid_sphere->y = rb->position.y;
        rb->rigid_sphere->z = rb->position.z;
    }


    for(int i = 0; i < gPhysicsWorld.count; i++) {
        for(int j = i + 1; j < gPhysicsWorld.count; j++) {
            A = &gPhysicsWorld.bodies[i];
            B = &gPhysicsWorld.bodies[j];
            float didcol = 0;
            if(A->owner->rigidbody_data && B->owner->rigidbody_data){
                if(A->rigid_sphere && B->rigid_sphere){
                    circle_circle_check(A, B, &normal);
                }
            }

            //Two rigidbodies collided
            if((normal.x || normal.y) || normal.z){
                //Calculate R.V.
                //If A is zero then 
                gfc_vector3d_sub(relativeVelocity, A->velocity, B->velocity);
                //slog("A->v: %f, %f B->v: %f, %f", A->velocity.x, A->velocity.y, B->velocity.x, B->velocity.y);
                velNormal = gfc_vector3d_dot_product(relativeVelocity, normal);
                //slog("%f", velNormal);
                //No clue but this breaks it
                if (velNormal < 0){
                    velNormal *=-1;
                }
                
                e = A->bounciness < B->bounciness ? A->bounciness: B->bounciness;
                impulseScalar = -(1 + e) * velNormal / (A->mass_inverse + B->mass_inverse);
                gfc_vector3d_scale(impulse, normal, impulseScalar);

                gfc_vector3d_scale(temp, impulse, A->mass_inverse);
                gfc_vector3d_add(A->velocity, A->velocity, temp);
                gfc_vector3d_scale(temp, impulse, B->mass_inverse);
                gfc_vector3d_sub(B->velocity, B->velocity, temp);
                
                /*Collision Correction*/
               
                float percent = .80f;
                float slop = 0.2f;
                 
                distance.x = B->rigid_sphere->x - A->rigid_sphere->x;
                distance.y = B->rigid_sphere->y - A->rigid_sphere->y;
                distance.z = B->rigid_sphere->z - A->rigid_sphere->z;
                float squaredDist = distance.x*distance.x+distance.y*distance.y+distance.z*distance.z;
                squaredDist = sqrtf(squaredDist);
                float penetration = A->rigid_sphere->r + B->rigid_sphere->r- squaredDist;
                if (penetration > slop) {
                    if(normal.z < 0){normal.z *= -2;}
                    float correctionMagnitude = (penetration - slop) / (A->mass_inverse + B->mass_inverse) * percent;

                    GFC_Vector3D correction;
                    gfc_vector3d_scale(correction, normal, correctionMagnitude);

                    GFC_Vector3D A_corr, B_corr;

                    gfc_vector3d_scale(A_corr, correction, A->mass_inverse);
                    gfc_vector3d_scale(B_corr, correction, B->mass_inverse);
                    if(A_corr.z <0)
                    A_corr.z *= 0; 
                    if(B_corr.z <0)
                    B_corr.z *= 0;
                    //A_corr.x *=2;B_corr.x *=2;A_corr.y *=2;B_corr.y *=2;
                    //A_corr.x *=-1;B_corr.x *=-1;A_corr.y *=-1;B_corr.y *=-1;
                    gfc_vector3d_sub(A->position, A->position, A_corr);
                    gfc_vector3d_add(B->position, B->position, B_corr);
                    //slog("x: %f y: %f \nx: %f y:%f",A_corr.x, A_corr.y,B_corr.x,B_corr.y); 
                }
            } 

            //We want to check and do the same funny against the ground
            if(A->owner->rigidbody_data && A->rigid_sphere){
                //circle_ground_check(A);
            }

        }
    }

    for (int i = 0; i < gPhysicsWorld.count; i++) {
        rb = &gPhysicsWorld.bodies[i];
        if(gfc_stricmp("Alien Guy", rb->owner->name) != 0){
            //NOT alien
            gfc_vector3d_copy(rb->owner->velocity, rb->velocity);
            gfc_vector3d_copy(rb->owner->position, rb->position);
        }
        else{
            //gfc_vector3d_copy(rb->velocity, rb->owner->velocity);
            //rb->owner->velocity.z = rb->velocity.z;
            gfc_vector3d_copy(rb->position, rb->owner->position);
        }
    }

    //Sync back
    
}
Uint8 body_get_floor_position(Entity *entity, World *world, GFC_Vector3D *contact){
    GFC_Vector3D contactHold;
    GFC_Vector3D downCheck, down;
    int x, mp=0;
    float footoffset = 4.91;
    downCheck = entity->position;
    downCheck.z -= 50000;
    gfc_vector3d_add(down, entity->position, gfc_vector3d(0,0,3));
    x = world_edge_test(world_get_the(), downCheck, down, &contactHold);
    contact->z = contactHold.z;
    return x;
}
void body_apply_gravity(Rigidbody *self){

    GFC_Vector3D *contact;
    //int hitFloor;//, i;
    float moveValid = 0;
    Uint8 hitfloor;
    float initalZ = self->velocity.z;
    //Self is rb
    contact = malloc(sizeof(GFC_Vector3D));

    hitfloor = body_get_floor_position(self->owner, world_get_the(), contact);


    //Did we hit the moving platform too?
    if(self->velocity.z > -10){
    self->velocity.z += GRAVITY;}

    //Movevalid is the distance you can move- this is bad, it skips
    moveValid = validate_move_between(self->velocity.z, self->position.z, contact->z, self->owner);
    //ITs just velocity? It should hit at some point

    if(moveValid != self->velocity.z){
        self->velocity.z = 0;
        
    }
    //slog("MV: %f Contact: %f", moveValid,  contact->z);
    
    // Uint8 t = self->position.z == contact->z;
    // slog("%f, pos %f, con %f", t, self->position.z, contact->z);
    if (fabs(moveValid) >= .0001){
        /*
            If you can move vertically, move vertically
            zero out our horizontal v for a second?
            When youre on the ground and you didnt jump dont fall just yet
            Techinally this is not a deliverable so we should just ignore it- move on
        */
        self->position.z += moveValid;
        
        if(moveValid != self->velocity.z && fabs(initalZ) > .01){
            //slog("waaa %f %f", self->velocity.z, initalZ);
            self->velocity.z = initalZ*-.9;
            self->position.z += self->velocity.z;
        }
    }
    //slog("what");
    if(!hitfloor){
        //slog("Here? %f, %f", self->velocity.x, self->velocity.y);
        //To make it interesting you only flip one of them
        self->outOfBounds++;
        hit_wall(self);
        //self->velocity.x *= -1;
        //self->velocity.y *= -1;
        //self->position.x += self->velocity.x*2;
        //self->position.y += self->velocity.y*2;
        
        //slog("Here? %f, %f", self->velocity.x, self->velocity.y);
    }
    else{self->outOfBounds = 0;}

    if(self->outOfBounds > 10){
        //slog("Freeing meee");
        //free(self->rigid_sphere);
        //entity_free(self->owner);
        set_position_to_origin(self);
    }

}
Uint8 circle_ground_check(Rigidbody *rb){
    //We want to check to see if it hits the ground
    //So we want to check if it hits the bottom mesh- lets export the mesh so we get just the floor?
    //No, thats stupid, lets do all of it
    //slog("Velocity z: %f", rb->velocity.z);
    /*if(rb->position.z < -25){
        rb->velocity.z*=-1;
        if(rb->position.z + 25 < .1 || rb->position.z + 25 >-.1)
            rb->velocity.z = 0;
    }*/
   return 0;
}

void hit_wall(Rigidbody *self){
    //Hitting a wall is when you will deflect but the other object won't and you know that
    //Aka a platform, wall, or out of bounds
    //OHH ok so we are out of bounds rn, good!      
    Uint8 hitfloor, flipX = 0, flipY = 0;
    GFC_Vector3D *contact;
    contact = malloc(sizeof(GFC_Vector3D));
    float initX, initY;
    initX = self->position.x;
    initY = self->position.y;
    //flipX and flipY are misleading, they are the opposite of what we say, so
    //notice they are flipped in the last checks
    self->position.x += self->velocity.x*-2;
    self->owner->position.x = self->position.x;
    hitfloor = body_get_floor_position(self->owner, world_get_the(), contact);
    if(!hitfloor){flipX = 1;}
    self->owner->position.x = initX;

    self->position.y += self->velocity.y*-2;
    self->owner->position.y = self->position.y;
    hitfloor = body_get_floor_position(self->owner, world_get_the(), contact);
    if(!hitfloor){flipY = 1;}
    self->owner->position.y = initY;

    if(!flipX){self->velocity.x *=-1;}
    if(!flipY){self->velocity.y *=-1;}
    self->position.x = initX + self->velocity.x*2;
    self->position.y = initY + self->velocity.y*2;

}
void physics_world_close(){
    int i;
    for(i = 0; i < gPhysicsWorld.maxBodies; i++){
        //if(gPhysicsWorld.bodies[i]._inuse){
            //entity_free(&entity_system.entity_list[i]);
        //}
        if(gPhysicsWorld.bodies[i]._inuse){
            rigidbody_free(&gPhysicsWorld.bodies[i]);
        }
    }
    //free(entity_system.entity_list);
    free(gPhysicsWorld.bodies);
}

/*
void body_validate_position(Rigidbody *rb){

    float x, y, z;
    x = rb->position.x + rb->velocity.x;
    y = rb->position.y + rb->velocity.y;
    z = rb->position.z + rb->velocity.z;

    //Check to see if its generally bound
    //TODO greater than like 100,000 dont move, or set the xyz

}*/