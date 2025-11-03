#include "simple_logger.h"
#include "entity.h"
#include "world.h"

Entity *mp_spawn(GFC_Vector3D position, GFC_Color color);
void mp_think(Entity *self);
void mp_update(Entity *self);
Uint8 mp_edge_test(Entity *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact);
