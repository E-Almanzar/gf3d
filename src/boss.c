#include "boss.h"
#include "rigidbody.h"
#include "physicsworld.h"
//Cannon boss? He shoots evilballs?

float timesincelastshot = 0;

void cannon_update(Entity *self){
    
}

void cannon_think(Entity *self){
    //Shoot an evilball?
    GFC_Vector3D spawnPos;
    Entity* evilball;
    if(timesincelastshot < 10){
    float xDir = (gfc_random()-.5)*10;
    spawnPos = gfc_vector3d(self->position.x+xDir, self->position.y, self->position.z + 100);
    //spawnPos = gfc_vector3d(0,0,0);

    evilball = evilball_spawn(spawnPos, GFC_COLOR_RED, gfc_vector3d(0,-100,10));
    physics_world_add(*(Rigidbody*)evilball->rigidbody_data);
    timesincelastshot++;
    }
}

Entity *cannon_spawn(GFC_Vector3D position, GFC_Color color){
    //slog("Here ?");
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    
    self->mesh = gf3d_mesh_load("models/boss/cannon.obj");
    self->texture = gf3d_texture_load("models/boss/cannon.png");
    strcpy(self->mesh->filename, "models/boss/cannon.obj");

    self->position = position;

    self->update = cannon_update;
    self->think = cannon_think;

    self->color = color;
    self->scale = gfc_vector3d(100,100,100);
    self->rotation.z = -GFC_HALF_PI;

    return self;
}