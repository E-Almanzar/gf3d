#include "simple_logger.h"
#include "entity.h"
#include "world.h"
#include "monster.h"

int i = 0;

void bp_update(Entity *self){
    
}

void bp_think(Entity *self){
    Entity *target;
    if(!self){return;}
    target = player_get_the();
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

    self->mesh = gf3d_mesh_load("models/bouncepad/bouncepad2.obj");
    self->texture = gf3d_texture_load("models/bouncepad/bouncepad.png");
    strcpy(self->mesh->filename, "models/bouncepad/bouncepad2.obj");
    self->scale = gfc_vector3d(1,1,1);
    self->color = color;
    self->position = position;
    self->think = bp_think;
    self->update = bp_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    slog("Position? %f, %f, %f", position.x, position.y, position.z);
    self->bounds->x = position.x-8;
    self->bounds->y = position.y-8;
    self->bounds->z = position.z;

    self->bounds->w = 16;
    self->bounds->h = 16;
    self->bounds->d = 16;
    
    strcpy(self->name, "bp");
    return self;
}


