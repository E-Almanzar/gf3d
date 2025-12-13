#include "boss.h"
#include "rigidbody.h"
#include "physicsworld.h"
#include "monster.h"
#include "monster_thinks.h"

//Cannon boss? He shoots evilballs?

int timesincelastshot = 0;
int allowShots = true;
void cannon_update(Entity *self){
    Entity *target;
    if(!self){return;}
    target = entity_check_collide(self, 3);
    if(target == NULL){return;} 
    if(target){
        slog("huh %s", target->name);
        //target->velocity.z = 1.5;
        //set_think_to_dead(target);
        entity_free(target);
    }
}

void cannon_think(Entity *self){
    //Shoot an evilball?
    GFC_Vector3D spawnPos;
    Entity* evilball;
    Entity* player;
    timesincelastshot++;

    player = player_get_the();
    if(allowShots){
        if(timesincelastshot % 100 == 0 && player->think != dead_think){
            float xDir = (gfc_random()-.5)*10;
            spawnPos = gfc_vector3d(self->position.x+xDir, self->position.y, self->position.z + 100);
            //spawnPos = gfc_vector3d(0,0,0);

            evilball = evilball_spawn(spawnPos, GFC_COLOR_RED, gfc_vector3d(0,-100,10));
            physics_world_add(*(Rigidbody*)evilball->rigidbody_data);
        }
    }
    else if(player->think == dead_think){
        timesincelastshot = 0;
        allowShots = true;
    }

    if(timesincelastshot > 10000){
        allowShots = false;
    }
}

Entity *cannon_spawn(GFC_Vector3D position, GFC_Color color){
    //slog("Here ?");
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    
    self->mesh = gf3d_mesh_load("models/boss/cannon.obj");
    self->texture = gf3d_texture_load("models/boss/cannon.png");
    strcpy(self->mesh->filename, "models/boss/cannon.obj");

    self->position = position;

    self->update = cannon_update;
    self->think = cannon_think;

    self->color = color;
    self->scale = gfc_vector3d(100,100,100);
    self->rotation.z = -GFC_HALF_PI;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-100;
    self->bounds->y = position.y-100;
    //We set the bounds low and maybe when we get you, and if we collide with the other
    //box you die? Thats kinda genius
    self->bounds->z = position.z;

    //Set the bounds to the bottom and send it out 40 
    self->bounds->w = 300;
    self->bounds->h = 300; //this is a fucking lie
    self->bounds->d = 300; //This is the real height :eyeroll:
    return self;
}