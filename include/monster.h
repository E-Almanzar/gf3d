#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "entity.h"


Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color);
void monster_set_cam(Entity* self, Entity *cam);
Entity* player_get_the();

//?


GFC_Vector3D player_get_forward();
void set_think_to_bounce(Entity *self);

#endif