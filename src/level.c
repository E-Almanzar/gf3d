#include "level.h"
#include "world.h"
#include "physicsworld.h"
#include "monster.h"
#include "entity.h"
typedef struct Level_Sky{
    Uint16      level_Index;
    Mesh        *sky_mesh;
    Texture     *sky_texture;
}Level_Sky;
Level_Sky *sky;
//Level
typedef struct Level{

    Entity      **level_entity_list;
    long int         num_ents;
    //Each level has just 1 sky and mesh
    World       *world; 
    //Level_Sky   *Sky;
} Level;


typedef struct Level_Manager{
    Level       *levels;
    int         currentLevel;
}Level_Manager;
Level_Manager *level_manager;


void level_manager_create(int num_Levels) {
    if (!level_manager) {
        level_manager = malloc(sizeof(Level_Manager));
        if (!level_manager) return;

        level_manager->levels = calloc(num_Levels, sizeof(Level));
        if (!level_manager->levels) {
            //free(level_manager);
            level_manager = NULL;
            return;
        }
        slog("Both allocated");
    }
    for(int i = 0; i < num_Levels; i++){
        level_manager->levels->level_entity_list = malloc(15 * sizeof(Entity*));
    }
}

void spawn_level_main_menu(){
        slog("Main menu");
        if (!level_manager || !level_manager->levels) {
        slog("Level manager not initialized!");
        return;
    }
   
    if(!sky){
        sky = malloc(sizeof(Level_Sky));
    }
    sky->sky_texture = gf3d_texture_load("models/sky/sky5.png");
    sky->sky_mesh = gf3d_mesh_load("models/sky/sky2.obj");
    world_load("defs/level3.def");
    ///Entity *player = player_get_the();
    set_think_to_dead(player_get_the());
    level_manager->currentLevel = 0;
}
void spawn_level_one() {
    world_load("defs/terrain.def");
    if (!level_manager || !level_manager->levels) {
        slog("Level manager not initialized!");
        return;
    }
    if(!sky){
        sky = malloc(sizeof(Level_Sky));
    }
    sky->sky_texture = gf3d_texture_load("models/sky/sky5.png");
    sky->sky_mesh = gf3d_mesh_load("models/sky/sky2.obj");
    level_manager->currentLevel = 1;
    /*

    int index = 0;
    GFC_Vector3D startpos;

    // Movepad
    startpos = gfc_vector3d(40, 20, -10);
    mp_spawn(startpos, GFC_COLOR_WHITE);

    // Bouncepads
    startpos = gfc_vector3d(-25, 0, -30.5);
    bp_spawn(startpos, GFC_COLOR_WHITE);

    // Teleporters
    startpos = gfc_vector3d(80, 0, -30.5);
    tp_spawn(startpos, GFC_COLOR_WHITE, 1);
    startpos = gfc_vector3d(-70, 0, -30.5);
    tp_spawn(startpos, GFC_COLOR_WHITE, 1);

    // Enemies
    startpos = gfc_vector3d(100, 100, 10);
    saucer_spawn(startpos, GFC_COLOR_WHITE);

    startpos = gfc_vector3d(-100, -100, 10);
    plant_spawn(startpos, GFC_COLOR_WHITE);

    // Powerups
    startpos = gfc_vector3d(0, -20, -28);
    powerup_spawn(startpos, GFC_COLOR_WHITE, 0);
    startpos = gfc_vector3d(-20, -20, -28);
    powerup_spawn(startpos, GFC_COLOR_WHITE, 0);
    startpos = gfc_vector3d(0, 20, -28);
    powerup_spawn(startpos, GFC_COLOR_WHITE, 1);
    startpos = gfc_vector3d(-20, 20, -28);
    powerup_spawn(startpos, GFC_COLOR_WHITE, 1);
*/
}

void spawn_level_two(){
    slog("Level 2 is spawned");
        if (!level_manager || !level_manager->levels) {
        slog("Level manager not initialized!");
        return;
    }
   
    if(!sky){
        sky = malloc(sizeof(Level_Sky));
    }
    sky->sky_texture = gf3d_texture_load("models/sky/sky.png");
    sky->sky_mesh = gf3d_mesh_load("models/sky/sky2.obj");
    world_load("defs/level2.def");
    level_manager->currentLevel = 2;


}
void spawn_level_three(){
    slog("Level 3 is spawned");
        if (!level_manager || !level_manager->levels) {
        slog("Level manager not initialized!");
        return;
    }
   
    if(!sky){
        sky = malloc(sizeof(Level_Sky));
    }
    sky->sky_texture = gf3d_texture_load("models/sky/sky5.png");
    sky->sky_mesh = gf3d_mesh_load("models/sky/sky2.obj");
    world_load("defs/level3.def");
    level_manager->currentLevel = 3;
}

void spawn_level(Uint16 ID){
    Level *level = &level_manager->levels[ID];
    if (!level) {
        slog("Invalid level ID %d", ID);
        return;
    }else{slog("In level %i", ID);}


    switch(ID){
    case 0: spawn_level_main_menu();
        return;
    case 1: spawn_level_one();
        return;
    case 2: spawn_level_two();
        return;
    case 3: spawn_level_three();
        return;
    }
    slog("No level dummy");

    //TODO we can make it different per levels
}


void draw_this_sky(Uint16 ID){
    //Level *level = &level_manager->levels[ID];
    GFC_Matrix4 skyMat;

    gfc_matrix4_identity(skyMat);
    //gfc_matrix4_scale(skyMat, skyMat, gfc_vector3d(1.2,1.02,1.02));
    //Entity *player = player_get_the();
    //gfc_matrix4_rotate_z(skyMat, skyMat, player->rotation.z);
    gf3d_sky_draw(sky->sky_mesh, skyMat, GFC_COLOR_WHITE, sky->sky_texture);

}

void spawn_next_level(){
    //Cleanup on aisle- this level
    //Delete all the ents besides the player?
    //Change the sky
    //clear the physics system?
    //Or just move everything out of view lol
    physics_world_close();
    physics_world_init(1000);
    entity_system_close();
    entity_system_init(1024);
    //world_free();
    GFC_Vector3D startpos =  gfc_vector3d(0,0,-20); 
    //slog("after both?");
    Entity *player, *camera;
    player = monster_spawn(startpos, GFC_COLOR_WHITE);  
    camera = camera_entity_spawn(startpos, player);
    monster_set_cam(player, camera);
    GFC_Vector3D cam = {0,10,10};
    gf3d_camera_look_at(gfc_vector3d(0,0,0),&cam);
    //PLAYER?
    spawn_level(level_manager->currentLevel+1);

}