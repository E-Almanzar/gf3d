#include "powerup.h"

float initalZ; 
float movestep = .01;

void p_long(Entity *self, Entity *target){
    if(target->scale.y != 1){
        //slog("maeke small now?");
        target->bounds->d = 1;
        target->bounds->x = target->position.x-4;
        target->bounds->y = target->position.y-4;
        target->bounds->z = target->position.z-4;
        target->speed = 1;
        target->scale = gfc_vector3d(1,1,1);
    }
    else if (target->scale.y == 1){
        //slog("make beeg");
        target->bounds->d = 10;
        target->bounds->x = target->position.x-4;
        target->bounds->y = target->position.y-4;
        target->bounds->z = target->position.z-4;
        target->speed = 1.5;
        target->scale = gfc_vector3d(1,10,1);
    }
}

//A lot of this code is in monster- search hardcoded
void p_mini(Entity *self, Entity *target){
    if(target->scale.x == .1f){
        target->bounds->x = target->position.x-4;
        target->bounds->y = target->position.y-4;
        target->bounds->z = target->position.z-4;
        target->scale = gfc_vector3d(1,1,1);
    }
    else{
        target->scale = gfc_vector3d(.1,.1,.1);
    }
}

void powerup_update(Entity *self){

    Entity *target;//, *pair;
    //pair = malloc(sizeof(Entity));
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
        target->velocity.z += .1;
        //slog("We powered up");
        
        if(gfc_stricmp("p_long", self->name) == 0){
            p_long(self, target);
        }
        else if(gfc_stricmp("p_mini", self->name) == 0){
           p_mini(self, target);
        }
        //Maybe we need to re-enable ourselves after a certain time?

        entity_free(self);
    }

}

void powerup_think(Entity *self){
    //slog("%f", movestep);
    self->rotation.z += .01;
    self->position.z += movestep;
    if(self->position.z> (initalZ+2) || self->position.z < (initalZ-2)){
        movestep *=-1;
    }

}

Entity *powerup_spawn(GFC_Vector3D position, GFC_Color Color, Uint8 flag){
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    //startPosPlant = gfc_vector3d(-100,-100,10); 
    self->mesh = gf3d_mesh_load("models/powerup.obj");
    self->texture = gf3d_texture_load("models/enemies/saucer.png");
    strcpy(self->mesh->filename, "models/enemies/saucer.png");

    self->scale = gfc_vector3d(.5,.5,.5);
    self->position = position;

    self->think = powerup_think;
    self->update = powerup_update;


    //The plant has radius 40 and height 20
    //Centered at the bottom middle
    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-5;
    self->bounds->y = position.y-5;
    //We set the bounds low and maybe when we get you, and if we collide with the other
    //box you die? Thats kinda genius
    self->bounds->z = position.z;

    //Set the bounds to the bottom and send it out 40 
    self->bounds->w = 10;
    self->bounds->h = 10; //this is a fucking lie
    self->bounds->d = 20; //This is the real height :eyeroll:

    if(flag == 0){
        self->color = GFC_COLOR_WHITE;
        strcpy(self->name, "p_long");
    }
    if(flag == 1){
        self->color = GFC_COLOR_RED;
        strcpy(self->name, "p_mini");
    }
    initalZ = position.z;
    return self;
}