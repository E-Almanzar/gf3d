#ifndef __BUG_H__
#define __BUG_H__
#include "simple_logger.h"
#include "entity.h"
#include "monster_thinks.h"
#include "monster.h"

void bug_update(Entity *self){
 Entity *target;
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
        //slog("huh");
        //target->velocity.z = 1.5;
        set_think_to_dead(target);
    }
}
GFC_Vector3D  startpos;  
float moveVal = .1;
void bug_think(Entity *self){
    self->bounds->x = self->position.x-8.5;
    self->bounds->y = self->position.y-8.5; //this is a fucking lie
    self->bounds->z = self->position.z;
    
    
    Entity* player = player_get_the();
    if(player->think != dead_think){
        GFC_Vector3D sPos, pPos;
        if(!player){slog("ideally we should free ourselves"); return;}
        sPos = self->position; pPos = player->position;
        //How do we slowly approach the player for a set amount of time then die?
        //In theory we should check for collisions but nahhhh

        if(fabs(sPos.x - pPos.x) >= .25){
            if(sPos.x > pPos.x){
                //Youre x is Greater; subtract to approach
                self->position.x -= moveVal;
            }else{
                self->position.x += moveVal;
            }
        }

        if(fabs(sPos.y - pPos.y) >= .25){
            if(sPos.y > pPos.y){
                self->position.y -= moveVal;
            }else{
                self->position.y += moveVal;
            }
        }
            
        if(fabs(sPos.z - (pPos.z-5.5)) >= .25){
            if(sPos.z > (pPos.z-5.5)){
                self->position.z -= moveVal;
            }else{
                self->position.z += moveVal;
            }
        }
        self->rotation.z += .01;
    }
    else{ //If the player is dead go back to your spawnPos
        self->position = startpos;
    }
}

Entity *bug_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    startpos = gfc_vector3d(-100,-100,10); 
    self->mesh = gf3d_mesh_load("models/enemies/bug.obj");
    self->texture = gf3d_texture_load("models/bug.png");
    strcpy(self->mesh->filename, "models/enemies/bug.obj");

    self->color = Color;
    self->position = position;

    self->think = bug_think;
    self->update = bug_update;


    //The bug has radius 40 and height 20
    //Centered at the bottom middle
    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-7;
    self->bounds->y = position.y-8.5;
    //We set the bounds low and maybe when we get you, and if we collide with the other
    //box you die? Thats kinda genius
    self->bounds->z = position.z;

    //Set the bounds to the bottom and send it out 40 
    self->bounds->w = 17;
    self->bounds->h = 17; //this is a fucking lie
    self->bounds->d = 10; //This is the real height :eyeroll:

    strcpy(self->name, "bug");

    return self;
}

#endif