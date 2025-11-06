#ifndef __SAUCER_H__
#define __SAUCER_H__
#include "simple_logger.h"
#include "entity.h"

float angle = 0;

void saucer_update(Entity *self){


}


void saucer_think(Entity *self){

    //GFC_Vector3D rotAbout = gfc_vector3d(0,0,0);
    //gfc_vector3d_rotate_about_z(&rotAbout, angle);
    //angle += 0.0174533;
}

Entity *saucer_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;

    self->mesh = gf3d_mesh_load("models/enemies/saucer2.obj");
    self->texture = gf3d_texture_load("models/enemies/saucer.png");
    strcpy(self->mesh->filename, "models/enemies/saucer2.obj");

    self->color = Color;
    self->position = position;

    self->think = saucer_think;
    self->update = saucer_update;


    //The saucer has radius 40 and height 20
    //Centered at the bottom middle
    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-20;
    self->bounds->y = position.y-20;
    self->bounds->z = position.z;

    //Set the bounds to the bottom and send it out 40 
    self->bounds->w = 40;
    self->bounds->h = 20;
    self->bounds->d = 40;

    strcpy(self->name, "Saucer");

    return self;
}

#endif