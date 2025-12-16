#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "entity.h"
#include "camera_entity.h"
typedef struct
{
    Entity *cam;
    CameraEntityData    *camData;
    GFC_Vector3D        forward;
    Uint8               jumpAllowed;
    Uint8               sprinting;
    int                 collected;
    Entity              **hats;
} MonsterEntityData; // Padding?
typedef struct player_anim{
    Mesh        **mesh_list;
    Texture     **texture_list;
    Uint16      iterator;
}player_anim;

Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color);
void monster_set_cam(Entity* self, Entity *cam);
Entity* player_get_the();

//?
void monster_think(Entity *self);
void monster_update(Entity *self);

MonsterEntityData * get_data_from_player();

void monster_gravity(Entity *self);
GFC_Vector3D player_get_forward();
void monster_move(Entity *self, Uint8 calledByPushback);
void monster_control(Entity *self);
void set_think_to_bounce(Entity *self, Uint8 flag);
void set_think_for_movement(Entity *self, Uint8 flag);
void set_think_to_dead(Entity *self);
void monster_collect(Entity *self, Entity *target);

long long get_timer_from_player();
int monster_get_collected();
void monster_buy_hat();
void hat_update();
#endif