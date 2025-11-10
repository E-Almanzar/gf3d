#include "powerup.h"

float initalZ; 
float movestep = .01;
void powerup_update(Entity *self){

    Entity *target;//, *pair;
    //pair = malloc(sizeof(Entity));
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
        target->velocity.z += .1;
        slog("We powered up");
        
        
        
        //Maybe we need to re-enable ourselves after a certain time?
        if(target->scale.y == 10){
            slog("maeke small now?");
            target->bounds->d = 1;
            target->speed = 1;
            target->scale = gfc_vector3d(1,1,1);
        }
        else{
            slog("make beeg");
            target->bounds->d = 10;
            target->speed = 1.5;
            target->scale = gfc_vector3d(1,10,1);
        }
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

Entity *powerup_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    //startPosPlant = gfc_vector3d(-100,-100,10); 
    self->mesh = gf3d_mesh_load("models/powerup.obj");
    self->texture = gf3d_texture_load("models/enemies/saucer.png");
    strcpy(self->mesh->filename, "models/enemies/saucer.png");

    self->scale = gfc_vector3d(.5,.5,.5);
    self->color = Color;
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

    strcpy(self->name, "p_");
    initalZ = position.z;
    return self;
}