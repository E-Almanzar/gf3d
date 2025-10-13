#include "simple_logger.h"
#include "entity.h"
//List of mesh pointers to create a bunch of objs
typedef struct{
    Entity *entity_list;
    Uint32 entity_max;

}EntitySystem;

static EntitySystem entity_system = {0};
/*
    @brief get a new pointer to a blank entity
    @return NULL on out of memory or other error a pointer to a blank entity otherwise
*/
Entity *entity_new(){
    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse){
            //  Set all defaults here
            //ALll the things that cant just be 0, color scale 
            entity_system.entity_list[i]._inuse = 1;
            entity_system.entity_list[i].color = GFC_COLOR_WHITE;
            entity_system.entity_list[i].scale = gfc_vector3d(1,1,1);
            //entity_system.entity_list[i].velocity = gfc_vector3d(0,0,0);
            entity_system.entity_list[i].speed = 0;
            entity_system.entity_list[i].doGenericUpdate = 1;
            // How do we set a default scale?
            return &entity_system.entity_list[i];
        } 
    
    }
    return NULL;
}

/*
    @brief free a previously new'd entity
    @param ent the entity to be free'd
    @note memory address should no longer be used
*/
void entity_free(Entity *ent){
    if(!ent){return;}
    //mesh and texture free?
    //This design pattern exist many places so we need to find it

}

void entity_system_close(Entity *ent){
    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(entity_system.entity_list[i]._inuse){
            entity_free(&entity_system.entity_list[i]);
        }

    }
    free(entity_system.entity_list);
}
/*
    @
*/
void entity_system_init(Uint32 max_ents){
    if (!max_ents) {slog("Max ents not defined");return;}
    entity_system.entity_list = gfc_allocate_array(sizeof(Entity), max_ents);
    if(!entity_system.entity_list){slog("bad");return;}
    entity_system.entity_max = max_ents;
    
    // atexit(entity_system_close()); ??? What was the at exit?
    // some kind of function pointer?
}


/*
    Function pointers
*/
//a void pointer for entity draw?

void entity_draw(Entity *ent, GFC_Vector3D lightPos, GFC_Color lightColor){
//Mesh *mesh,GFC_Matrix4 modelMat,GFC_Color mod, Texture *texture, GFC_Vector3D lightPos, GFC_Color lightColor
    //Use the function that will create the matrix from the vectors pos rot and scale
    if(!ent) return;
    GFC_Matrix4 modelMat;
    gfc_matrix4_from_vectors(
        modelMat,
        ent->position,
        ent->rotation,
        ent->scale);
    gf3d_mesh_draw(
        ent->mesh,
        modelMat,
        ent->color, 
        ent->texture, 
        lightPos, //????
        lightColor
    );
    //entity system init before srand sdl
}

void entity_system_draw_all(GFC_Vector3D lightPos, GFC_Color lightColor){

    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse){continue;}
            //If its in use draw it
            // if not in use continue?
            entity_draw(&entity_system.entity_list[i], lightPos, lightColor);
        
    }
}

//void entity_system_think_all();

void entity_think(Entity *ent){

    if(!ent) return;
    if(ent->think) ent->think(ent);

}

void entity_system_think_all(){

    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse){continue;}
            //If its in use draw it
            // if not in use continue?
            entity_think(&entity_system.entity_list[i]);
        
    }
}
//void entity_system_update_all();


void entity_update(Entity *ent){

    if(!ent) return;
    if(ent->update) ent->update(ent);

}

void entity_system_update_all(){

    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse){continue;}
            //If its in use draw it
            // if not in use continue?
            entity_update(&entity_system.entity_list[i]);
        
    }
}

