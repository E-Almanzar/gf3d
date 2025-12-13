#ifndef __GOAL_H__
#define __GOAL_H__
#include "simple_logger.h"
#include "entity.h"

//    Entity *ent;
    Entity *goal_spawn(GFC_Vector3D position, GFC_Color color);
void goal_set_key(int flag);

#endif