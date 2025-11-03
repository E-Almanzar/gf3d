#include "simple_logger.h"
#include "entity.h"
#include "world.h"
#include "monster.h"


void bp_update(Entity *self){

}

void bp_think(Entity *self){
    Entity *target;
    if(!self){return;}
    //We check through the entity list to see if their box collides with our box?
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
        //Bounce em
        set_think_to_bounce(target, 0);
    }

    
}


Entity *bp_spawn(GFC_Vector3D position, GFC_Color color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    self->mesh = gf3d_mesh_load("models/bouncepad/bouncepad.obj");
    self->texture = gf3d_texture_load("models/bouncepad/bouncepad.png");
    strcpy(self->mesh->filename, "models/bouncepad/bouncepad.obj");
    self->scale = gfc_vector3d(10,10,10);
    self->color = color;
    self->position = position;
    self->position.y -= 12;
    self->think = bp_think;
    self->update = bp_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);

    self->bounds->x = position.x-12;
    self->bounds->y = position.y-27;
    self->bounds->z = position.z;
    self->bounds->w = 27;
    self->bounds->h = 27;
    self->bounds->d = 15;

    strcpy(self->name, "bp");
    return self;
}


