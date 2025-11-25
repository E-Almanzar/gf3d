#include "rigidbody.h"

//Exern?
extern PhysicsWorld gPhysicsWorld;

void rigidbody_on_collide(Entity *self, GFC_Vector3D normal){
    //slog("we on collided biatch");
    //What we want to do is modify its velocity based 
    //OK we have to deal with the zero normal case
    /*if(!normal.x){normal.x = .00001;}
    if(!normal.y){normal.y = .00001;}
    if(!normal.z){normal.z = .00001;}
    slog("%f, %f, %f ", normal.x, normal.y, normal.z);
    //How do we deal with energy tranfser? We have one thing not moving and one thing moving
    self->velocity = gfc_vector3d_multiply(normal, self->velocity);
    /
   if(gfc_stricmp("Alien Guy", self->name) != 0){
    ((struct Rigidbody*)self->rigidbody_data)->velocity.z+=10;
    slog("%f: %f", self->velocity.z,  ((struct Rigidbody*)self->rigidbody_data)->velocity.z);
   }*/
}

//Lowercase r rigidbody is the entity, and big R is the rigidbody data aka not that
void rigidbody_update(Entity *self){

    //Convert decisions into physics inputs.
    // entity does not write position
    // entity only writes velocity / forces

}

void rigidbody_think(Entity *self){
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

Entity *rigidbody_spawn(GFC_Vector3D position, GFC_Color color){
    //slog("Here ?");
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
    self->collide = rigidbody_on_collide;

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
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = position.x-5;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = position.y-5;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = position.z;
    ((struct Rigidbody*)self->rigidbody_data)->onFloor = 0;

    //iNITAL 
    if(self->position.x == 0){
        self->velocity.x = .1;
        strcpy(self->name, "Blue");
    }
    else{
        self->color = GFC_COLOR_RED;
        //self->velocity.x;
        ((struct Rigidbody*)self->rigidbody_data)->mass_inverse = .5;
        //self->scale.x*=11;self->scale.y*=11;self->scale.z*=11;
        strcpy(self->name, "Red");

    }
    return self;
}