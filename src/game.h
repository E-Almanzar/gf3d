#ifndef __GAME_H__
#define __GAME_H__

#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_camera.h"
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

#include "level.h"
#include "physicsworld.h"
long long get_timer();

#endif