#include "simple_logger.h"
#include "entity.h"
#include "world.h"
#include "monster.h"


void bp_update(Entity *self){

}

void bp_think(Entity *self){
    //We check through the entity list to see if their box collides with our box?
    Entity *target;
    GFC_Vector3D forward;
    //slog("Hello?");
    if(!self){return;}

    //Run it with yourself and the player
    //ents_mesh_collide_check(self, player_get_the());
   
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    //if(target){slog("target found in bp");}
    //slog("%f, %f, %f", target->velocity.x, target->velocity.y, target->velocity.z);
    //target = player_get_the();
    if(target){
        //Bounce em
        forward = player_get_forward(target);
        //gfc_vector3d_add(target->velocity, target->velocity, forward);

        //forward.z is now the rotation?
        //starget->rotation.z = atan2(forward.y,forward.y);
        set_think_to_bounce(target);
        //slog("target name %s", target->name);
        //target->velocity.z = 30;
        /*slog(
            "\nBp Pos: %f, %f, %f \nAlien Pos: %f, %f, %f \nBP BB POS %f, %f, %f \n 1 out %f, %f, %f", 
            self->position.x, self->position.y, self->position.z,
            target->position.x,target->position.y,target->position.z,
            self->bounds->x,self->bounds->y,self->bounds->z,
            target->bounds->x,target->bounds->y,target->bounds->z
            );*/
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
    //We need to set the bounds positions to the corner-
    //This is kinda stupid idk how to make it all the way around the box
    //Should I just make the mesh?
    self->bounds->x = position.x-12;
    self->bounds->y = position.y-27;
    self->bounds->z = position.z;
    self->bounds->w = 27;
    self->bounds->h = 27;
    self->bounds->d = 15;
    strcpy(self->name, "bp");
    return self;
}


