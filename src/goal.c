#include "simple_logger.h"
#include "entity.h"
#include "monster.h"
#include "simple_json.h"
#include "gfc_config.h"
#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "world.h"
//int p =0;
int boss_key = 0;
void goal_update(Entity *self){
    Entity *target;
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target && boss_key){
    //slog("We get it %i", p++);
    /*
    *   Request the level manager to change levels based on the current level
    */
    slog("Changing levels"); 
   }

}
//0 is boss, 1 is collectables
void goal_set_key(int flag){
    if(!flag){
        boss_key = true;
    }
    if(flag==1){
        //collect_key = true;
    }

}

void goal_think(Entity *self){

    self->bounds->x = self->position.x-14;
    self->bounds->y = self->position.y-14;
    self->bounds->z = self->position.z;

}


Entity *goal_spawn(GFC_Vector3D position, GFC_Color color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    self->mesh = gf3d_mesh_load("models/cube.obj");
    //self->texture = gf3d_texture_load("models/bouncepad/movepad.png");
    strcpy(self->mesh->filename, "models/cube.obj");
    //self->scale = gfc_vector3d(4,4,4);
    self->color = color;
    self->position = position;
    self->think = goal_think;
    self->update = goal_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    self->bounds->x = position.x-14;
    self->bounds->y = position.y-14;
    self->bounds->z = position.z;
    self->bounds->w = 28;
    self->bounds->h = 28;
    self->bounds->d = 28;
    //14x14x14? from the center

    strcpy(self->name, "goal");
    return self;
}

