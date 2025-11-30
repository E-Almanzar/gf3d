#include "simple_logger.h"
#include "entity.h"
#include "gf3d_obj_load.h"
#include "world.h"
#include "monster.h"
#include "gf3d_mesh.h"
#include "m_plat.h"
#include "tp.h"
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
/*
    //real freaking cheap ass shadows
    //a new thing in entity
    //Tell it the flag Uint8 in the entity to draw the shadow
    void entity_draw_shadow(Entity *ent){
        if(!ent || !ent->drawshadow) return;
        GFC_Matrix4 modelMat;
        GFC_Vector3D drawPos;
        gfc_vector3d_copy(drawPos, ent->position);
        drawpos.z += .1;
        gfc_matrix4_from_vectors(
            modelMat,
            ent->position,
            ent->rotation,
            gfc_vector3d(ent->scale.x, ent->scale.y, .01);
        gf3d_mesh_draw(
            ent->mesh,
            modelMat,
            gfc_color(0,0,0,120),
            //ent->texture,
            gfc_vector3d(0,0,0) 

        );


    }
*/

Uint8 mp_edge_test2(Entity *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact){
    //UGHHH above checks gross
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

    //slog("start: %f, %f, %f, end: %f, %f, %f", start.x, start.y, start.z, end.x, end.y, end.z);

    for(i = 0; i < c; i++){
        prim = gfc_list_nth(world->mesh->primitives, i);
        //if primitive bad continue
        if(!prim){
            continue;
        }
        d = prim->objData->face_count;
        for(j = 0; j <= d; j++){
            /* 
                t is a triangle and its looking for a b and c,
                a b and c are vector3ds
            
            */
            t.a = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            t.b = prim->objData->faceVertices[prim->objData->outFace[j].verts[1]].vertex;
            t.c = prim->objData->faceVertices[prim->objData->outFace[j].verts[2]].vertex;
            //t.a prim obj faceVertices[ prim obj outFace [j].verts[0,1,2].vertex;
            //int x = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            //slog("ugh trigangle %f, %f, %f", t.a, t.b, t.c);
            //GFC_Edge3D e = gfc_edge3d_from_vectors(gfc_vector3d(0,0,0),gfc_vector3d(0,0,0));
            if(gfc_trigfc_angle_edge_test(e,t,contact)) {
                //slog("~~~~~~~~~~~~~~~~returning True");
                return 1;
            }
            //if(gfc_point_in_box(world->bounds))
            //slog("triangle edge test failed i:%i j:%i dc %i, %i", i, j,d,c);
        }
        

    }
    return 0;
}

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
            
            //Bounding Box- Uh oh
            entity_system.entity_list[i].bounds = NULL;
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
    if(ent->free)ent->free(ent);
    gf3d_mesh_free(ent->mesh);
    gf3d_texture_free(ent->texture);
    memset(ent,0,sizeof(Entity));

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
    World *world;
    world = world_get_the();
    gfc_matrix4_from_vectors(
        modelMat,
        ent->position,
        ent->rotation,
        ent->scale);
    //slog("Here i am? drawing %s", ent->name);
    gf3d_outline_draw(
        ent->mesh,
        modelMat,
        ent->color, 
        ent->texture, 
        world->lightPos, //????
        lightColor
    );
    gf3d_mesh_draw(
        ent->mesh,
        modelMat,
        ent->color, 
        ent->texture, 
        world->lightPos, //????
        lightColor
    );
    //entity_draw_shadow(ent);
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

Uint8 world_mp_edge_helper(GFC_Vector3D *downCheck, GFC_Vector3D *down, GFC_Vector3D *contact){
    int i, x;
    //GFC_Vector3D contact;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse){continue;}
        if(gfc_stricmp("bp", entity_system.entity_list[i].name) == 0){
            //slog("in the check?");
            x = mp_edge_test2(&entity_system.entity_list[i], *downCheck, *down, contact);
            if(x)
             return x;
        }
    }
    return 0;
}

Uint8 entity_get_floor_position(Entity *entity, World *world, GFC_Vector3D *contact){
    GFC_Vector3D contactHold;//, contactHold2;
    GFC_Vector3D downCheck, down;//, footpos;
    int x, mp=0;
    float footoffset = 4.91;
    downCheck = entity->position;
    downCheck.z -= 50000; //Increase if distance to floor needs to
    
    //TODO why does this happen, what do we need- ask in the discord
    //is this a vector not moving 
    //down.x =0; down.y =0; down.z = 0;
    //instead of abovehead we need down
    gfc_vector3d_add(down, entity->position, gfc_vector3d(0,0,3));
    x = world_edge_test(world_get_the(), downCheck, down, &contactHold);
    //mp = world_mp_edge_helper(&downCheck, &down, &contactHold2);
    /*if(x && mp){
        if(contactHold.z < contactHold2.z){
        contact->x = contactHold2.x;
        contact->y = contactHold2.y;
        contact->z = contactHold2.z+footoffset;
        slog("contacthold2 baby");
        }
    }else{
    contact->x = contactHold.x;
    contact->y = contactHold.y;
    contact->z = contactHold.z+footoffset;
    }*/
    //default to ground
    if(!mp){
        contact->x = contactHold.x;
        contact->y = contactHold.y;
        contact->z = contactHold.z+footoffset;
    }
    //contact = &contactHold;
    //slog("%f, %f, %f",contact->x, contact->y, contact->z);
    //gfc_vector3d_copy(contactHold, gfc_vector3d(contact->x, contact->y, contact->z+footoffset));
    //slog("%f, %f, %f",contact->x, contact->y, contact->z);
    

    //gfc_vector3d_copy(footpos, gfc_vector3d(contact->x, contact->y, contact->z));
    /*
    if(x){
        gfc_vector3d_copy(footpos, gfc_vector3d(contact->x, contact->y, contact->z+footoffset));
        //No more snapping- maybe only return here?
        //gfc_vector3d_copy(entity->position,footpos);
        //slog("Self Position: %f,%f,%f", entity->position.x, entity->position.y, entity->position.z);
        //slog("Contact: %i, %f, %f, %f", x, contact.x, contact.y, contact.z);
        //return contact;
    }
        //return world_edge_test(entity,entity->position, down, contact);
    //return x;
    return x;*/
    if(x || mp)
        return 1;
    return 0;
}

Uint8 mesh_primitive_collide_check(MeshPrimitive *prim1, MeshPrimitive *prim2, Entity *one, Entity *two){    
    if(!prim1 || !prim2){slog("f you, no prim in mprim collide check"); return 0;}
    //Create a vertex in triangle check?
    GFC_Triangle3D t;
    //Do we have to make an edge between each of the two points of the triangle?
    GFC_Edge3D e1, e2, e3;
    //prim one point 1, prim 2 point 1
    GFC_Vector3D p1p1, p1p2, p1p3;
    //GFC_Vector3D p2p1, p2p2, p2p3;
    int i, j, c, d;
    GFC_Vector3D *contact1, *contact2, *contact3;
    contact1 = malloc(sizeof(GFC_Vector3D));
    contact2 = malloc(sizeof(GFC_Vector3D));
    contact3 = malloc(sizeof(GFC_Vector3D));

    c = prim1->objData->face_count;
    //For every face in prim 1
    for(i = 0; i < c; i++){
        //Set the p1p...
        //getting the three points of the first primitive's vertex
        p1p1 = prim1->objData->faceVertices[prim1->objData->outFace[i].verts[0]].vertex;
        p1p2 = prim1->objData->faceVertices[prim1->objData->outFace[i].verts[1]].vertex;
        p1p3 = prim1->objData->faceVertices[prim1->objData->outFace[i].verts[2]].vertex;
        //Then create three edges
        e1 = gfc_edge3d_from_vectors(p1p1, p1p2);
        e2 = gfc_edge3d_from_vectors(p1p2, p1p3);
        e3 = gfc_edge3d_from_vectors(p1p1, p1p3); 
        //Then set d
        d = prim2->objData->face_count;
        for(j = 0; j < d; j++){

            t.a = prim2->objData->faceVertices[prim2->objData->outFace[j].verts[0]].vertex;
            t.b = prim2->objData->faceVertices[prim2->objData->outFace[j].verts[1]].vertex;
            t.c = prim2->objData->faceVertices[prim2->objData->outFace[j].verts[2]].vertex;
            //t.a prim obj faceVertices[ prim obj outFace [j].verts[0,1,2].vertex;
            //int x = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
           
            //Kind of brute force way to test all of the vectors
            //If any of the triangles hit the one side of the other return true
            //if(gfc_point3d_in_primitive()...?
            //    if (gfc_point_in_triangle(intersectPoint,t))
//!!



            if(gfc_trigfc_angle_edge_test(e1,t,contact1)) {
                //printf("e1: %f, %f, %f\n", contact1->x, contact1->y, contact1->z);
                return 1;
            }
            if(gfc_trigfc_angle_edge_test(e2,t,contact2)) {
                //printf("e2: %f, %f, %f\n", contact2->x, contact2->y, contact2->z);

                return 1;}
            if(gfc_trigfc_angle_edge_test(e3,t,contact3)) {
                //printf("e3: %f, %f, %f\n", contact3->x, contact3->y, contact3->z);
                return 1;}
        }        
    }
    //Is it creating like, infinite lines betweens them so it always hits it?
    //How do i get just the meshes?
    return 0;
}
Uint8 ents_mesh_collide_check(Entity *one, Entity *two){
    if(!one || !two){slog("no ents in mesh collide check"); return 0;}
    GFC_List *oneL, *twoL;
    int i, j, c, d, test;
    oneL = one->mesh->primitives;
    c = gfc_list_count(oneL);
    twoL = two->mesh->primitives;
    for(i = 0; i <  c; i++){
        d = gfc_list_count(twoL);
        for(j = 0; j < d; j++){
            //If they overlap (HOW check?)
            //return 1;
            //For every primitive
                //mesh_primitive_collide_check(oneP, twoP);
            //Or just send in every the lists? nahh
            //all on the zero zero?
            test = mesh_primitive_collide_check(gfc_list_nth(oneL, i), gfc_list_nth(twoL, j), one, two);
            if(test){
                //slog("yes in ents mesh collide %i, i:%i, j:%i", test, i, j);
                //slog("%s, %s", one->name, two->name);
                return test;}
        }
    }
    return 0;
}
/*
Uint8 entity_to_entity_test(){
    
    for(i = 0; i < c; i++){
        prim = gfc_list_nth(world->mesh->primitives, i);
        //if primitive bad continue
        if(!prim){
            continue;
        }
        d = prim->objData->face_count;
        for(j = 0; j < d; j++){
            /
                t is a triangle and its looking for a b and c,
                a b and c are vector3ds
            
            /
            t.a = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            t.b = prim->objData->faceVertices[prim->objData->outFace[j].verts[1]].vertex;
            t.c = prim->objData->faceVertices[prim->objData->outFace[j].verts[2]].vertex;
            //t.a prim obj faceVertices[ prim obj outFace [j].verts[0,1,2].vertex;
            //int x = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
           
            //GFC_Edge3D e = gfc_edge3d_from_vectors(gfc_vector3d(0,0,0),gfc_vector3d(0,0,0));
            if(gfc_trigfc_angle_edge_test(e,t,contact)) {
                //slog("returning True");
                return 1;
            }
           //slog("triangle edge test failed i:%i j:", i, j);
        }
}
*/
//Bugged as hell, idk why its returning when it shouldnt be
//Use the new function to get the player and see if its the same

//Flags
//0 is player, 1 is bouncepad
Entity *entity_check_collide(Entity *self, Uint16 flag){
    int i;
    //if(flag == 1){slog("looking for a bouncepad");}

    if(!self){slog("no self in entity check collide"); return NULL;}
    if(!self->bounds){slog("You dont have a freaking box"); return NULL;}
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse || !entity_system.entity_list[i].bounds){continue;}
        //We check with our box and the ents box
        //So now we have our box and the targets box
        if(gfc_box_overlap(*self->bounds,*entity_system.entity_list[i].bounds)){
            //slog("WE OVERLAP!!! %s", entity_system.entity_list[i].name);
            //if(strcmp("Alien Guy",entity_system.entity_list[i].name)){
            //K so this is also wrong? IDK
            
            if(flag == 0){
                if(player_get_the() == &entity_system.entity_list[i]){
                    //slog("alien?");
                    return &entity_system.entity_list[i];
                }
            }
            
            if(flag == 1){
                //For a mving plat, if you collide with it then do something?
                if(gfc_stricmp(entity_system.entity_list[i].name, "mp") == 0){
                    //slog("returning %s",entity_system.entity_list[i].name );
                    return &entity_system.entity_list[i];
                }
            }
        }
    }
    return NULL;
}

Entity *entity_find_tp_partner(Entity *pair, Entity *self,Uint16 tpPair){
    int i;
    for(i = 0; i < entity_system.entity_max; i++){
        if(!entity_system.entity_list[i]._inuse || !entity_system.entity_list[i].data){continue;}
        //stricmp for tp's only
        //slog("name %s", entity_system.entity_list[i].name);
        if(gfc_stricmp(entity_system.entity_list[i].name, "tp") == 0){

            if(&entity_system.entity_list[i] != self){
                if(((struct TeleportData*)entity_system.entity_list[i].data)->tpPair == tpPair){
                //if()    
                    //slog("find tp partner");
                    pair = &entity_system.entity_list[i];
                    return &entity_system.entity_list[i];
                }
            }
        }
    }
    return NULL;
}

float validate_move_between(float velocity, float entpos, float targetpos, Entity *self)
{
    //Stupid- if its zero it stops
    /*if (!velocity || !entpos || !targetpos)
    {
        slog("Nothing in validate move");
        return 0;
    }*/
    /*if(targetpos > 1000000 || targetpos < 1000000){
        slog("Nothing in validate move for targetpos");
        return 0;
    }*/
    float distance = entpos - targetpos;
    if (fabs(distance) > fabs(velocity))
    {
        // slog("difference, velocity %f, >  %f, %i", fabs(distance), fabs(velocity), fabs(distance) > fabs(velocity));
        return velocity;
    }

    /*
        If the difference is less than or equal to velocity- theres a problem
        calculate the allowed distance

        do we set the v velocity to zero here?

    */
    self->velocity.z = 0;
    // slog("Distance: %f", distance);
    if (velocity < 0)
        return -1 * distance;
    return distance;
}