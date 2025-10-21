#ifndef __WORLD_H__
#define __WORLD_H__
#include "simple_json.h"
#include "gf3d_mesh.h"
#include "gfc_primitives.h"


typedef struct {
    Mesh *          mesh;
    Texture         *texture;
    GFC_List        entities;
    GFC_Vector3D    lightPos;
    GFC_Color       lightColor;

}World;

World * world_new();
World * world_load(const char *name);
World * world_get_the();
void world_draw(World * world,GFC_Matrix4 mat);
#endif