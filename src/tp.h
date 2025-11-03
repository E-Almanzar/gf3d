#include "simple_logger.h"
#include "entity.h"
#include "world.h"
typedef struct TeleportData
{
    Uint16  tpPair;
} TeleportData;
Entity *tp_spawn(GFC_Vector3D position, GFC_Color color, Uint16 tpPair);
void tp_think(Entity *self);
void tp_update(Entity *self);
Uint8 tp_edge_test(Entity *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact);
