#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_matrix.h"
//#include "world.h"
#include "gf3d_mesh.h"
#include "entity.h"
#include "monster.h"
#include "bouncepad.h"
//#include "camera_entity.h"
#include "m_plat.h"
#include "tp.h"

#include "saucer.h"
#include "bug.h"
#include "plant.h"

#include "powerup.h"
void spawn_level(Uint16 ID);
void spawn_level_one();
void spawn_level_two();
void level_manager_create(int num_Levels);
void draw_this_sky(Uint16 ID);
void spawn_next_level();
#endif