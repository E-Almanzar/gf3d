#include "simple_logger.h"
#include "entity.h"
#include "world.h"

Entity *bp_spawn(GFC_Vector3D position, GFC_Color color);
void bp_think(Entity *self);
void bp_update(Entity *self);