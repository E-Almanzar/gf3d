#include "gfc_text.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"
#include "world.h"
#include "gf3d_mesh.h"
#include "gf3d_texture.h"

typedef struct CEntData_S{
    Entity          *target;
    float           angle;
    float           vangle;
    float           followHeight;
    float           followDist;
    void            (*free)(struct CEntData_S *self);
    //We need a forward
    GFC_Vector3D    forward;

}CameraEntityData;

void camera_entity_think(Entity *self);
Entity *camera_entity_spawn(GFC_Vector3D position, Entity *target);
void camera_entity_free(CameraEntityData *self);