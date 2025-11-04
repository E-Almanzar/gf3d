#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "entity.h"
#include "camera_entity.h"
typedef struct
{
    Entity *cam;
    CameraEntityData *camData;
    GFC_Vector3D forward;
    Uint8 jumpAllowed;
} MonsterEntityData; // Padding?

Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color);
void monster_set_cam(Entity* self, Entity *cam);
Entity* player_get_the();

//?
void monster_think(Entity *self);
void monster_update(Entity *self);

MonsterEntityData * get_data_from_player();


GFC_Vector3D player_get_forward();
void set_think_to_bounce(Entity *self, Uint8 flag);
long long get_timer_from_player();

#endif