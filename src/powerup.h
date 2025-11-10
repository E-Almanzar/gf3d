#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "simple_logger.h"
#include "monster.h"
#include "monster_thinks.h"

Entity *powerup_spawn(GFC_Vector3D position, GFC_Color Color, Uint8 flag);

#endif