#include "rigidbody.h"

//Exern?
extern PhysicsWorld gPhysicsWorld;

void rigidbody_update(Entity *self){

}

void rigidbody_think(Entity *self){
    
}

Entity *rigidbody_spawn(GFC_Vector3D position, GFC_Color color){
    slog("Here ?");
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    
    // Actually thats pretty good for a circle
    self->mesh = gf3d_mesh_load("models/powerup.obj");
    self->texture = gf3d_texture_load("models/enemies/saucer.png");
    strcpy(self->mesh->filename, "models/enemies/saucer.png");

    //self->scale = gfc_vector3d(.5,.5,.5);
    self->position = position;

    self->update = rigidbody_update;
    self->think = rigidbody_think;


    self->s_bounds = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    self->s_bounds->x = position.x-5;
    self->s_bounds->y = position.y-5;
    self->s_bounds->z = position.z;
    self->s_bounds->r = 1;

    self->data = gfc_allocate_array(sizeof(Rigidbody), 1);
    ((struct Rigidbody*)self->data)->mass_inverse = 0;
    ((struct Rigidbody*)self->data)->bounciness = 1;
    return self;
}