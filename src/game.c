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
#include "camera_entity.h"

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}


int main(int argc,char *argv[])
{
    //local variables
    GFC_Matrix4 dinoM;
    GFC_Matrix4 id, skyMat, terrainMat;
    GFC_Vector3D lightdir;
    GFC_Vector3D startpos;

    Entity *player;
    Entity *camera;

    //OG camera and movement
    float theta = 0;//, delta = .01;
    GFC_Vector3D cam = {0,10,10};
    //GFC_Vector3D lookingAt = gfc_vector3d(0,0,0);
    
    //Sky
    Mesh *sky_mesh;
    Texture *sky_texture;
    
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");

    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    
    //game init
    entity_system_init(1024);
    srand(SDL_GetTicks());
    slog_sync();
    gf2d_mouse_load("actors/mouse.actor");
    
    gfc_matrix4_identity(id);
    
    //Skybox
    gfc_matrix4_identity(skyMat);
    lightdir = gfc_vector3d(5,0,5); 
    startpos =  gfc_vector3d(0,0,-20); 
    sky_mesh = gf3d_mesh_load("models/sky/sky.obj");
    sky_texture = gf3d_texture_load("models/sky/sky3.png");
    gfc_matrix4_scale(skyMat, skyMat, gfc_vector3d(.78, .78, .78)); //It was clipping the far plane

    //mesh = gf3d_mesh_load("models/dino/dino.obj");
    //texture = gf3d_texture_load("models/dino/dino.png");
    //gfc_matrix4_identity(id);
    //if(mesh){slog("Mesh in game.c");}



    // First is horizontal, then depth? then vertical
    //
    
    //theta = 1;
    /*for(int i = 0; i < 1023; i++){
        monster_spawn(gfc_vector3d(gfc_crandom()*150,gfc_crandom()*100,gfc_crandom()*20), gfc_color(gfc_random(), gfc_random(), gfc_random(), 1));
    }
    
    
    
    
    //lookingAt.z -= GFC_HALF_PI;
    //lookingAt.x -= GFC_PI;
    */

    //World
    World * world = world_load("defs/terrain.def");
    gfc_matrix4_multiply_scalar(terrainMat, id, 5);
    //Camera Spawn
    //gf3d_camera_look_at(gfc_vector3d(0,0,0),&cam);
    /*for(int i = 0; i < 100; i++){
        monster_spawn(gfc_vector3d(gfc_crandom()*150,gfc_crandom()*100,gfc_crandom()*20), gfc_color(gfc_random(), gfc_random(), gfc_random(), 1));
    }*/
    player = monster_spawn(startpos, GFC_COLOR_WHITE);    
    camera = camera_entity_spawn(startpos, player);
    monster_set_cam(player, camera);
    gf3d_camera_look_at(gfc_vector3d(0,0,0),&cam);

    //main loop: everything in here is run repeatedly
    while(!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        // Think all and update all
        entity_system_think_all();
        entity_system_update_all();
        
        //world updates
        /*theta += delta;
        if(theta > 1 || theta < -1){
            delta *= -1;
        }
        //lookingAt.x = theta; 
        //lookingAt.z = theta;
        //lookingAt.y = theta;
        */

        /*if(fabs(lookingAt.z) > GFC_PI){
            //lookingAt.z = -GFC_HALF_PI;

        }*/
        //x moves up and down, and z left and right- lock it so that you dont see the skybox hole
        //lookingAt.x += theta;
        //lookingAt.z += theta;
        //gf3d_camera_set_rotation(lookingAt);
        //slog("%f: %f, %f, %f",theta, lookingAt.x, lookingAt.y, lookingAt.z);
        //slog("%f, %f",theta, delta);
        //gf3d_camera_look_at(lookingAt,&cam);
        //cam.y+=theta;
        theta += .01;

        gfc_matrix4_rotate_z(dinoM,id,theta);
        //camera updaes
        gf3d_camera_update_view();
        gf3d_vgraphics_render_start(); // No updates between render start and render end
                //3D draws
                //gf3d_mesh_draw(mesh,dinoM,GFC_COLOR_WHITE,texture,lightdir, GFC_COLOR_WHITE);
                
                
                //You need to make sure the edge test is edge testing?
                //Ask in the discord what the down thing should be?
                world_draw(world, terrainMat);// World is not being drawn rn
                gf3d_sky_draw(sky_mesh, skyMat, GFC_COLOR_WHITE, sky_texture);
                entity_system_draw_all(lightdir, GFC_COLOR_WHITE);

                //2D draws
                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                gf2d_mouse_draw();
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    exit(0);
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
