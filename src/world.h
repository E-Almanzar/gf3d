#ifndef __WORLD_H__
#define __WORLD_H__
#include "simple_json.h"
#include "gf3d_mesh.h"
#include "gfc_primitives.h"
#include "m_plat.h"
#include "bouncepad.h"
//#include "camera_entity.h"
#include "tp.h"

#include "saucer.h"
#include "bug.h"
#include "plant.h"

#include "powerup.h"
#include "rigidbody.h"
#include "physicsworld.h"
#include "goal.h"
typedef struct {
    Mesh *          mesh;
    Texture         *texture;
    GFC_List        entities;
    GFC_Vector3D    lightPos;
    GFC_Color       lightColor;

}World;

void spawn_by_types(SJson *ents);
World * world_new();
World * world_load(const char *name);
World * world_get_the();
void world_draw(World * world,GFC_Matrix4 mat);
Uint8 world_edge_test(World *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact);
#endif