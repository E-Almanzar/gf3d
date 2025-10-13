#include "simple_logger.h"
#include "monster.h"
#include "entity.h"


void monster_update(Entity *self){
    
    if(!self) return;
    //slog("Monster updating?");
    //self->position.z *= self->velocity.z;  
}

//velocity will just be x speed y speed and z speed
void monster_think(Entity *self){
    if(!self) return;
     
    //self->position.x += self->speed;
    self->position.x += self->velocity.x;
    //slog("Monster thinking?");

    self->rotation.z += self->speed;
    //self->rotation.z += .1;
    if(fabs(self->position.x) > 150){
        //self->velocity.x *= -1;
        self->position.x *=-1;
        self->speed *=-1;
    }
    //slog("x position: %.2f; x velocity: %.2f", self->position.x, self->velocity.x);
}

Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;
    /*
    self->mesh = gf3d_mesh_load("models/dino/dino.obj");
    self->texture = gf3d_texture_load("models/dino/dino.png");
    strcpy(self->mesh->filename, "models/dino/dino.obj");
    */
    
    self->mesh = gf3d_mesh_load("models/alien/alien.obj");
    self->texture = gf3d_texture_load("models/alien/alien.png");
    strcpy(self->mesh->filename, "models/alien/alien.obj");

    self->color = Color;
    self->position = position;
    self->think = monster_think;
    //self->update = monster_update;
    //self->velocity = position;
    // = gfc_vector3d(.25,0,0);
    
    //self->velocity.z = .25;

    self->velocity.x = .25;
    //slog("WHAT %f", self->velocity.x);
    self->speed = gfc_random()/10;
    //slog("WHAT %i", self->speed);
    self->rotation.z = 3.141592;
    
    slog("Creating %s", self->mesh->filename);
//Somewhere you need to pass the name!!!!!!111
    /*slog("checking everything mesh: %i, texture: %i, postion: %i",
        self->mesh != 0,
        self->texture != 0,
        self->mesh != 0
    );*/
    return self;
}


/*eol@eof*/