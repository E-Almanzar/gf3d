#include "simple_logger.h"
#include "entity.h"
#include "monster.h"
#include "simple_json.h"
#include "gfc_config.h"
#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "world.h"
#include "monster.h"
#include "gfc_input.h"
#include "dialogue.h"
#include "gf2d_mouse.h"

void dino_update(Entity *self){
    Entity *target;
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target && gf2d_mouse_button_pressed(0)){
        set_think_to_dialogue();
    }

}


void dino_think(Entity *self){


    //In theory it shouldnt move. We need animations too?
    self->bounds->x = self->position.x-14;
    self->bounds->y = self->position.y-14;
    self->bounds->z = self->position.z;

}

void anim_init_dino(Entity *self){
    slog("%zu", sizeof(player_anim));
    self->anim_data = gfc_allocate_array(sizeof(player_anim), 3);
    slog("%p", self->anim_data);
    ((struct player_anim*)self->anim_data)->mesh_list = gfc_allocate_array(sizeof(Mesh), 3);
    ((struct player_anim*)self->anim_data)->mesh_list[2] = gf3d_mesh_load("models/dino/Dino.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[1] = gf3d_mesh_load("models/dino/Dino2.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[0] = gf3d_mesh_load("models/dino/Dino3.obj");
    /*
    self->anim_data = gfc_allocate_array(sizeof(player_anim), 3);
    ((struct player_anim*)self->anim_data)->mesh_list = gfc_allocate_array(sizeof(Mesh), 3);
    ((struct player_anim*)self->anim_data)->mesh_list[0] = gf3d_mesh_load("models/dino/Dino.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[1] = gf3d_mesh_load("models/dino/Dino2.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[2] = gf3d_mesh_load("models/dino/Dino3.obj");

    self->mesh = ((struct player_anim*)self->anim_data)->mesh_list[0];
    self->texture = gf3d_texture_load("models/dino/dino.png");
    strcpy(self->mesh->filename, "models/dino/Dino.obj");
    */
    self->mesh = ((struct player_anim*)self->anim_data)->mesh_list[0];
    self->texture = gf3d_texture_load("models/dino/dino.png");
    strcpy(self->mesh->filename, "models/dino/Dino.obj");

}


Entity *dino_spawn(GFC_Vector3D position, GFC_Color color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    /*
    self->mesh = gf3d_mesh_load("models/dino/Dino.obj");
    //self->texture = gf3d_texture_load("models/bouncepad/movepad.png");
    strcpy(self->mesh->filename, "models/dino/Dino.obj");
    */
    //self->scale = gfc_vector3d(4,4,4);
    self->color = color;
    self->position = position;
    self->think = dino_think;
    self->update = dino_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    self->bounds->x = position.x-14;
    self->bounds->y = position.y-14;
    self->bounds->z = position.z;
    self->bounds->w = 28;
    self->bounds->h = 28;
    self->bounds->d = 28;
    //14x14x14? from the center
    anim_init_dino(self);
    self->scale = gfc_vector3d(5,5,5);

    strcpy(self->name, "Dino");
    return self;
}

