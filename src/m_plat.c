#include "simple_logger.h"
#include "entity.h"
#include "monster.h"
#include "simple_json.h"
#include "gfc_config.h"
#include "gfc_primitives.h"
#include "gf3d_obj_load.h"
#include "world.h"
#include "m_plat.h"
#include "monster_thinks.h"

//float moveBy = .1;
Uint8 mp_edge_test(Entity *world, GFC_Vector3D start, GFC_Vector3D end, GFC_Vector3D *contact);

void mp_update(Entity *self){
    float initalX = ((float*)self->data)[0];
    float position = ((float*)self->data)[2];
    //slog("%f, %f",initalX, self->position.x);
    //float initalY = ((float*)self->data)[1] = self->position.y;
    //float moveBy = ((float*)self->data)[2];
    self->position.x += self->velocity.x;
    if(self->position.x > (initalX+position) || self->position.x < (initalX-position)){
        self->velocity.x *= -1;
        //slog("%f, What??", self->velocity.x);
    }
    Entity *target;//, *pair;
    //pair = malloc(sizeof(Entity));
    if(!self){return;}
    target = entity_check_collide(self, 0);
    if(target == NULL){return;} 
    if(target){
        //slog("We hit it");
        if(target->think == glide_think){
            set_think_for_movement(target, 3);
        }
        target->position.z = self->position.z+8;
        
        //target->velocity.z = -.00001;
        //slog("target %f", target->velocity.z);
        //slog("tf??");
    }
    //((float*)self->data)[2] = moveBy;
    

}

void mp_think(Entity *self){
    /*Entity *target;
    GFC_Vector3D forward;
    if(!self){return;}

    target = entity_check_collide(self);
    if(target == NULL){return;} 

    if(target){

    }
    */
    //float moveBy = ((float*)self->data)[2];

    //self->velocity.x;
    //slog("Moveby? %f", self->velocity.x);
    self->bounds->x = self->position.x-12;//
    self->bounds->y = self->position.y-14;
    self->bounds->z = self->position.z;

}


Entity *mp_spawn(GFC_Vector3D position, GFC_Color color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;

    self->mesh = gf3d_mesh_load("models/bouncepad/bouncepad3.obj");
    self->texture = gf3d_texture_load("models/bouncepad/movepad2.png");
    strcpy(self->mesh->filename, "models/bouncepad/bouncepad3.obj");
    //self->scale = gfc_vector3d(4,4,4);
    self->color = color;
    self->position = position;
    self->think = mp_think;
    self->update = mp_update;

    self->bounds = gfc_allocate_array(sizeof(GFC_Box),1);
    //We need to set the bounds positions to the corner-
    //This is kinda stupid idk how to make it all the way around the box
    //Should I just make the mesh?
    //self->bounds->x = position.x-12;
    //self->bounds->y = position.y-27;
    //self->bounds->z = position.z+10;
    self->bounds->w = 24;
    self->bounds->h = 24;
    self->bounds->d = 4;//?

    //Uh oh we did a fucky wucky and broke it :<

    self->data = gfc_allocate_array(sizeof(float),3);
    ((float*)self->data)[0] = self->position.x;
    ((float*)self->data)[1] = self->position.y;
    //((float*)self->data)[2] = .1;
    
    //float xDir = (gfc_random()-.5)*10;

    ((float*)self->data)[2] = (gfc_random()-.5)*10+10;
    self->velocity.x = .1;
    strcpy(self->name, "mp");
    return self;
}

Uint8 gfc_triangle_edge_testH(
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
            if(gfc_triangle_edge_testH(e,t,contact)) {
                //slog("~~~~~~~~~~~~~~~~returning True");
                return 1;
            }
            //if(gfc_point_in_box(world->bounds))
            //slog("triangle edge test failed i:%i j:%i dc %i, %i", i, j,d,c);
        }
        

    }
    return 0;
}