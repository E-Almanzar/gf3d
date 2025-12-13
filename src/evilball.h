#ifndef __EVILBALL_H__
#define __EVILBALL_H__

#include "simple_logger.h"
#include "entity.h"

Entity *evilball_spawn(GFC_Vector3D position, GFC_Color color, GFC_Vector3D initalVelocity);

#endif