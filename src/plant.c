#ifndef __plant_H__
#define __plant_H__
#include "simple_logger.h"
#include "entity.h"
#include "monster_thinks.h"
#include "monster.h"

void snap_to_ground(Entity *self);


void plant_update(Entity *self){
 Entity *target;
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
//        slog("huh");
        set_think_to_dead(target);
    
    }
}
GFC_Vector3D startPosPlant,  plantVelocityX, plantVelocityY;  
float moveValForPlant = .1;

void plant_think(Entity *self){
    self->bounds->x = self->position.x-2;
    self->bounds->y = self->position.y-2; //this is a fucking lie
    self->bounds->z = self->position.z;
    snap_to_ground(self);
    /*
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
                self->position.x -= moveValForPlant;
            }else{
                self->position.x += moveValForPlant;
            }
        }

        if(fabs(sPos.y - pPos.y) >= .25){
            if(sPos.y > pPos.y){
                self->position.y -= moveValForPlant;
            }else{
                self->position.y += moveValForPlant;
            }
        }
            
        if(fabs(sPos.z - (pPos.z-5.5)) >= .25){
            if(sPos.z > (pPos.z-5.5)){
                self->position.z -= moveValForPlant;
            }else{
                self->position.z += moveValForPlant;
            }
        }
        self->rotation.z += .01;
    }
    else{ //If the player is dead go back to your spawnPos
        self->position = startposPlant;
    }*/
}

Entity *plant_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    startPosPlant = gfc_vector3d(-100,-100,10); 
    self->mesh = gf3d_mesh_load("models/enemies/plant.obj");
    self->texture = gf3d_texture_load("models/dresser.png");
    strcpy(self->mesh->filename, "models/dresser.png");

    self->color = Color;
    self->position = position;

    self->think = plant_think;
    self->update = plant_update;


    //The plant has radius 40 and height 20
    //Centered at the bottom middle
    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-1;
    self->bounds->y = position.y-1;
    //We set the bounds low and maybe when we get you, and if we collide with the other
    //box you die? Thats kinda genius
    self->bounds->z = position.z;

    //Set the bounds to the bottom and send it out 40 
    self->bounds->w = 2;
    self->bounds->h = 2; //this is a fucking lie
    self->bounds->d = 10; //This is the real height :eyeroll:

    strcpy(self->name, "plant");

    return self;
}

void snap_to_ground(Entity *self)
{
    if (!self)
        return;
    GFC_Vector3D *contact;
    int didCont;
    contact = malloc(sizeof(GFC_Vector3D));

    didCont = entity_get_floor_position(self, world_get_the(), contact);
    //slog("Did contact %i, %f, %f, %f", didCont, contact->x, contact->y, contact->z);
    // gfc_vector3d_copy(self->position, gfc_vector3d(contact->x, contact->y, contact->z+4.91));
    self->position.z = contact->z-5;
    
    free(contact);
}

#endif

