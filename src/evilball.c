#include "rigidbody.h"
#include "evilball.h"
#include "monster.h"
#include "monster_thinks.h"
GFC_Vector3D startpos_evil;  
float moveVals = .77;


void evilball_on_collide(Entity *self, GFC_Vector3D normal){
    if(gfc_stricmp("Alien Guy",self->name) == 0){
        //slog("we hit?");
    }
}

void evilball_update(Entity *self){

    //Convert decisions into physics inputs.
    // entity does not write position
    // entity only writes velocity / forces


    //KILL!

}

void evilball_think(Entity *self){
    //We should expire after like 30 seconds?
    //Maybe our data is just one number thats a time

    if(self->velocity.z > -1){
    //self->velocity.z -=.001;
    }
    ((struct Rigidbody*)self->rigidbody_data)->velocity.x = self->velocity.x;
    ((struct Rigidbody*)self->rigidbody_data)->velocity.y = self->velocity.y;
    ((struct Rigidbody*)self->rigidbody_data)->velocity.z = self->velocity.z;

    Entity* player = player_get_the();
    if(player->think != dead_think){
        GFC_Vector3D sPos, pPos;
        if(!player){slog("ideally we should free ourselves"); return;}
        sPos = self->position; pPos = player->position;
        //How do we slowly approach the player for a set amount of time then die?
        //In theory we should check for collisions but nahhhh
        int flipX, flipY;
        flipX = flipY = 1;
        if(gfc_random() < .33){
            flipX*=-1;
        }
        if(gfc_random() < .33){
            flipY*=-1;
        }
        if(fabs(sPos.x - pPos.x) >= .25){
            if(sPos.x > pPos.x){
                //Youre x is Greater; subtract to approach
                //self->position.x -= moveVals;
                self->velocity.x -= moveVals*flipX;
            }else{
                //self->position.x += moveVals;
                self->velocity.x += moveVals*flipX;
            }
        }

        if(fabs(sPos.y - pPos.y) >= .25){
            if(sPos.y > pPos.y){
                //self->position.y -= moveVals;
                self->velocity.y -= moveVals*flipY;
            }else{
                //self->position.y += moveVals;
                self->velocity.y += moveVals*flipY;
            }
        }
        if((gfc_random() < .33)){
            /*if(fabs(sPos.z - (pPos.z-5.5)) >= .25){
                if(sPos.z > (pPos.z-5.5)){
                    self->position.z -= moveVals;
                }else{
                    self->position.z += moveVals;
                }
            }*/
        }
        self->rotation.z += .1;
    }
    else{
        entity_free(self); //If the player is dead go back to your spawnPos
        //self->position = startpos_evil;
    }
    //slog("%f, %f, %f", self->velocity.x, self->velocity.y, self->velocity.z);

}

Entity *evilball_spawn(GFC_Vector3D position, GFC_Color color, GFC_Vector3D initalVelocity){
    //slog("Here ?");
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    
    // Actually thats pretty good for a circle
    self->mesh = gf3d_mesh_load("models/enemies/evilball.obj");
    self->texture = gf3d_texture_load("models/alien/Alien2.png");
    strcpy(self->mesh->filename, "models/enemies/evilball.obj");

    self->position = position;

    self->update = evilball_update;
    self->think = evilball_think;
    self->collide = evilball_on_collide;
    

    self->s_bounds = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    self->s_bounds->x = position.x-5;
    self->s_bounds->y = position.y-5;
    self->s_bounds->z = position.z;
    self->s_bounds->r = 10;

    self->rigidbody_data = gfc_allocate_array(sizeof(Rigidbody), 1);
    ((struct Rigidbody*)self->rigidbody_data)->mass_inverse = 1;
    ((struct Rigidbody*)self->rigidbody_data)->bounciness = 1;
    ((struct Rigidbody*)self->rigidbody_data)->owner = self;
    gfc_vector3d_copy(((struct Rigidbody*)self->rigidbody_data)->position, self->position);
    
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->r = 10;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = position.z;
    ((struct Rigidbody*)self->rigidbody_data)->onFloor = 0;
    ((struct Rigidbody*)self->rigidbody_data)->friction = .67;

    self->color = color;
    //idgaf if it gets overwritten theyre all abt the same
    //gfc_vector3d_copy(startpos_evil, self->position);
    startpos_evil.x = 0;
    startpos_evil.y = 1500;
    startpos_evil.z = -70.5;
    self->velocity.x = initalVelocity.x;
    self->velocity.y = initalVelocity.y;
    self->velocity.z = initalVelocity.z;
    strcpy(self->name, "Evil Ball");
    
    return self;
}