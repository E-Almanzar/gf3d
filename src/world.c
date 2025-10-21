#include "simple_logger.h"

#include "simple_json.h"
#include "gfc_config.h"
#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "world.h"

static World * world; 

World * world_new(){
    World * world;
    world = gfc_allocate_array(sizeof(World), 1);
    //do init
    return world;
}
World * world_load(const char *name){
    slog("world load?");
    World * world;
    SJson *json;
    SJson *config;
    const char * str;

    json = sj_load(name);

    if(!json){
        slog("failed json");
        return NULL;
    }
    world = world_new();
    if(!world){
        slog("failed world");
        sj_free(json);
        return NULL;
    }
    config = sj_object_get_value(json, "world");
    if(!config){
        //free that shit
    }

    str = sj_object_get_string(config, "terrainMesh");
    world->mesh = gf3d_mesh_load(str);
    world->texture = gf3d_texture_load(sj_object_get_string(config, "terrainTexture"));
    sj_object_get_vector3d(config,"lightPos", &world->lightPos);

    world->lightColor = GFC_COLOR_WHITE;
    sj_free(json);
    return world;
}

void world_free(World * world){
    return;

}


void world_draw(World * world, GFC_Matrix4 mat){
    //GFC_Matrix4 modelMat;
    gf3d_mesh_draw(world->mesh, mat, GFC_COLOR_WHITE, world->texture, world->lightPos, world->lightColor);
}

//call this in your ent get floor position
Uint8 world_edge_test(World *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact){
    GFC_Edge3D e;
    int i, j, c, d;
    MeshPrimitive * prim;
    GFC_Triangle3D t; 

    if(!world){
        slog("no world in egde test"); 
        return 0;
    }
    
    e = gfc_edge3d_from_vectors(start, end);
    c = gfc_list_count(world->mesh->primitives);



    for(i = 0; i < c; i++){
        prim = gfc_list_nth(world->mesh->primitives, i);
        //if primitive bad continue
        if(!prim){
            continue;
        }
        d = prim->objData->face_count;
        for(j = 0; j < d; j++){
            /* 
                t is a triangle and its looking for a b and c,
                a b and c are vector3ds
            
            */
            t.a = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            t.b = prim->objData->faceVertices[prim->objData->outFace[j].verts[1]].vertex;
            t.c = prim->objData->faceVertices[prim->objData->outFace[j].verts[2]].vertex;
            //t.a prim obj faceVertices[ prim obj outFace [j].verts[0,1,2].vertex;
            //int x = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;

            /*if(gfc_triangle_edge_test(e,t,contact)) {
                slog("returning True");
                return 1;
            }*/
           slog("triangle edge test failed i:%i j:", i, j);
        }
        

    }
    return 0;
}

World * world_get_the(){
    return world;
}
/*
world * world get the 
return the world, put it in the header, and the moster can get the world in its think?
*/