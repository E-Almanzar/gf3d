#include "simple_logger.h"
#include "entity.h"
#include "monster.h"
#include "simple_json.h"
#include "gfc_config.h"
#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "world.h"
#include "tp.h"

long long global_tp_timer = -500;

void tp_update(Entity *self){
    
    
    //slog("bwuh");
    Entity *target, *pair;
    pair = malloc(sizeof(Entity));
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    //If weve captured the alien
    if(target){
        //Look for the matching tp Pair in the list of entities, and set target position to their 
        //Position, plus some offset 
        
        pair = entity_find_tp_partner(pair, self,((struct TeleportData*)self->data)->tpPair);
        //slog("We trying to teleported");
        if(!pair){return;}
        //Why is pair always null?
        //Pair now holds your partner
        //Why is it only when we jump? are we just going back and forth?
        //slog("We teleported to %f, %f, %f from %f, %f, %f", pair->position.x,pair->position.y, pair->position.z, target->position.x, target->position.y, target->position.z);
        //We get the timer from the player, and iff the difference is less than some numer of frames, we let them teleport

        //If its been 500 since the last tp, you can tp 
        //TODO: Make the timer display on screen
        if(get_timer_from_player() - global_tp_timer >= 500){
            target->position = pair->position;
            set_think_to_bounce(target, 1);
            //target->position.z += 20;
            //if(target->velocity.z < 0){target->velocity.z *= -1;}
            //target->velocity.z += 10;
            global_tp_timer = get_timer_from_player();

        }
    }



}

void tp_think(Entity *self){
    
    //self->velocity.x = moveBy;
    self->bounds->x = self->position.x-15;
    self->bounds->y = self->position.y-15;
    self->bounds->z = self->position.z;

}

/*FOR the teleporter, you need to initalize two of them with an equivalent new parameter
//This is the pair number, and when they match, set the players location to the other
pair's location
*/
Entity *tp_spawn(GFC_Vector3D position, GFC_Color color, Uint16 tpPair){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    self->mesh = gf3d_mesh_load("models/bouncepad/bouncepad.obj");
    self->texture = gf3d_texture_load("models/bouncepad/tp.png");
    strcpy(self->mesh->filename, "models/bouncepad/bouncepad.obj");
    self->scale = gfc_vector3d(10,10,10);
    self->color = color;
    self->position = position;
    self->position.y -= 12;
    self->think = tp_think;
    self->update = tp_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    //We need to set the bounds positions to the corner-
    //This is kinda stupid idk how to make it all the way around the box
    //Should I just make the mesh?
    //self->bounds->x = position.x-12;
    //self->bounds->y = position.y-27;
    //self->bounds->z = position.z+10;
    self->bounds->w = 33;
    self->bounds->h = 33;
    self->bounds->d = 15;

    //Set its tpPair
    self->data = gfc_allocate_array(sizeof(TeleportData), 1);
    ((struct TeleportData*)self->data)->tpPair = tpPair; 
    strcpy(self->name, "tp");
    return self;
}

Uint8 gfc_triangle_edge_test(
    GFC_Edge3D e,
    GFC_Triangle3D t,
    GFC_Vector3D *contact)
{
    float time;
    GFC_Plane3D p;
    GFC_Vector3D intersectPoint = {0,0,0};

    p = gfc_trigfc_angle_get_plane(t);
    time = gfc_edge_in_plane(e,p,&intersectPoint);
    if ((time <= 0)||(time > 1))
    {
        return 0;
    }
    if (gfc_point_in_triangle(intersectPoint,t))
    {
        if (contact)
        {
            gfc_vector3d_copy((*contact),intersectPoint);
        }
        //printf("RETURN 1 PLS");
        return 1;
    }
    return 0;
}
/*
Uint8 mp_edge_test(Entity *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact){
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
            /
                t is a triangle and its looking for a b and c,
                a b and c are vector3ds
            
            /
            t.a = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            t.b = prim->objData->faceVertices[prim->objData->outFace[j].verts[1]].vertex;
            t.c = prim->objData->faceVertices[prim->objData->outFace[j].verts[2]].vertex;
            //t.a prim obj faceVertices[ prim obj outFace [j].verts[0,1,2].vertex;
            //int x = prim->objData->faceVertices[prim->objData->outFace[j].verts[0]].vertex;
            //slog("ugh trigangle %f, %f, %f", t.a, t.b, t.c);
            //GFC_Edge3D e = gfc_edge3d_from_vectors(gfc_vector3d(0,0,0),gfc_vector3d(0,0,0));
            if(gfc_triangle_edge_test(e,t,contact)) {
                //slog("~~~~~~~~~~~~~~~~returning True");
                return 1;
            }
            //if(gfc_point_in_box(world->bounds))
            //slog("triangle edge test failed i:%i j:%i dc %i, %i", i, j,d,c);
        }
        

    }
    return 0;
}*/