#include "rigidbody.h"
#include "evilball.h"

void evilball_on_collide(Entity *self, GFC_Vector3D normal){

}

//Lowercase r rigidbody is the entity, and big R is the rigidbody data aka not that
void evilball_update(Entity *self){

    //Convert decisions into physics inputs.
    // entity does not write position
    // entity only writes velocity / forces

}

void evilball_think(Entity *self){
    //We REQUEST movement
    //

    if(self->velocity.z > -1){
    //self->velocity.z -=.001;
    }
    ((struct Rigidbody*)self->rigidbody_data)->velocity.x = self->velocity.x;
    ((struct Rigidbody*)self->rigidbody_data)->velocity.y = self->velocity.y;
    ((struct Rigidbody*)self->rigidbody_data)->velocity.z = self->velocity.z;
   //slog("%f, %f, %f", self->velocity.x, self->velocity.y, self->velocity.z);

}

Entity *evilball_spawn(GFC_Vector3D position, GFC_Color color){
    //slog("Here ?");
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    
    // Actually thats pretty good for a circle
    self->mesh = gf3d_mesh_load("models/enemies/evilball.obj");
    self->texture = gf3d_texture_load("models/alien/Alien2.png");
    strcpy(self->mesh->filename, "models/enemies/evilball.obj");

    self->position = position;

    self->update = evilball_update;
    self->think = evilball_think;
    self->collide = evilball_on_collide;

    self->s_bounds = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    self->s_bounds->x = position.x-5;
    self->s_bounds->y = position.y-5;
    self->s_bounds->z = position.z;
    self->s_bounds->r = 10;

    self->rigidbody_data = gfc_allocate_array(sizeof(Rigidbody), 1);
    ((struct Rigidbody*)self->rigidbody_data)->mass_inverse = 1;
    ((struct Rigidbody*)self->rigidbody_data)->bounciness = 1;
    ((struct Rigidbody*)self->rigidbody_data)->owner = self;
    gfc_vector3d_copy(((struct Rigidbody*)self->rigidbody_data)->position, self->position);
    
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->r = 10;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = position.z;
    ((struct Rigidbody*)self->rigidbody_data)->onFloor = 0;
    self->color = color;


    return self;
}