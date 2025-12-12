#include "simple_logger.h"
#include "entity.h"
#include "world.h"
#include "monster.h"
#include "rigidbody.h"

int i = 0;

void bp_update(Entity *self){
    
}

void bp_think(Entity *self){
    Entity *target;
    if(!self){return;}
    target = player_get_the();
    //We check through the entity list to see if their box collides with our box?
    target = entity_check_collide(self,2);
    //target = NULL;
    if(target == NULL){return;} 
    if(target){
        //Bounce em
        slog("what?");
        set_think_to_bounce(target, 0);
    }

    
}


Entity *bp_spawn(GFC_Vector3D position, GFC_Color color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    self->mesh = gf3d_mesh_load("models/bouncepad/bouncepad3.obj");
    self->texture = gf3d_texture_load("models/bouncepad/bouncepad3.png");
    strcpy(self->mesh->filename, "models/bouncepad/bouncepad3.obj");
    self->scale = gfc_vector3d(1,1,1);
    self->color = color;
    self->position = position;
    self->think = bp_think;
    self->update = bp_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    //slog("Position? %f, %f, %f", position.x, position.y, position.z);
    /*self->bounds->x = position.x-8;
    self->bounds->y = position.y-8;
    self->bounds->z = position.z;
    self->bounds->w = 16;
    self->bounds->h = 16;
    self->bounds->d = 16;

    */    
    self->bounds->x = self->position.x-12;//
    self->bounds->y = self->position.y-14;
    self->bounds->z = self->position.z;
    self->bounds->w = 24;
    self->bounds->h = 24;
    self->bounds->d = 4;//?
        
    self->rigidbody_data = gfc_allocate_array(sizeof(Rigidbody), 1);
    ((struct Rigidbody*)self->rigidbody_data)->mass_inverse = 0;
    ((struct Rigidbody*)self->rigidbody_data)->bounciness = 0;
    ((struct Rigidbody*)self->rigidbody_data)->owner = self;
    gfc_vector3d_copy(((struct Rigidbody*)self->rigidbody_data)->position, self->position);
    
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->r = 1;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = position.z;
    ((struct Rigidbody*)self->rigidbody_data)->onFloor = 0;

    strcpy(self->name, "bp");
    return self;
}


