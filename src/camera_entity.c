#include "simple_logger.h"
#include "gfc_input.h"
#include "gf3d_camera.h"
#include "entity.h"
#include "camera_entity.h"
#include "gf2d_mouse.h"

#define SMOOTH_FACTOR .9
GFC_Vector2D smoothedVel;
//relative mouse state?



/*
if(gfc_input_command_down("walkleft")){
    self->position.x += move

}
//pan self->rotation.x += rot


camera_entity_free 
*/

void camera_entity_think(Entity *self){
    /*define move pitch turn and yaw
    vector3D movement = {0};
    dir = {0};
    CameraEntityData *data;
    if(!self)or no data return
    sub dir data target, self pos 
    vector3d normalize &dir
    //big ass if block
        //vector3d scale(dir, dir, movement.y), add position, position, dir, and the line below, normalize, rotate about z by GFC_HALF_PI,
        //scale it by x 


    //move me and my tripod?
    gfc_vector3d_add(self->position, self->position,movement);
    gfc_vector3d_add(data->target, data->target,movement);

    if()

    if(pitch){
        data->target.z += pitch;


        //gfc_vector3d_rotate_about_z(&dir, pitch);
    }

    if(yaw){
        sub dir data target, self posix_memalign
        normalize dir
        rotate about z dir yaw
        vector3d add data target, 
    }*/


    //camera look at    
    /*
    // if no self or no self data return
    CameraEntity *data, vector3D dfollow height and follow distance
    data = self data
    //copy it? vector3d copy(d, from d->t->p)
    //offset = gfc_vector3d(0,1,0)
    //gfc_vector3d_rotate_about_z(offset, data->angle)
    gfc_vector3d_scale(offset,  offset,data->followDistance);
    offset.z = data->followHeight
    gfc_vector3d_add(self->position, data->target->position,offset)
    camera_look_at(data->target->position, &self->position)
*/
    //slog("ooh were camera entity thinking it");
    if((!self)||(!self->data))return;
    CameraEntityData *data;
    GFC_Vector3D offset;
    float followDist, angle, vangle, panStep = .05, vpanStep = .25;
    //followHeight,     followHeight = data->followHeight; //Maybe we can use this as max in either direction
    data = self->data;
    followDist = data->followDist;
    angle = data->angle;
    offset = gfc_vector3d(0,1,0);
    GFC_Vector2D mousePosition, mouseMovement;
    mouseMovement.x = 0; mouseMovement.y = 0;
    //Camera Movement
    /*
    if(gfc_input_command_down("panright")){
        angle += panStep;
    }
    if(gfc_input_command_down("panleft")){
        angle -= panStep;
    }*/
    angle = data->angle;
    vangle = data->vangle;
    mousePosition = gf2d_mouse_get_position();
    if(mousePosition.x <= 0){
        slog("We hit the x 0");
        SDL_WarpMouseInWindow(NULL, 1278, mousePosition.y);
                gf2d_mouse_update();

    }
    if(mousePosition.x >= 1279){
        slog("We hit the X max");
        SDL_WarpMouseInWindow(NULL, 1, mousePosition.y);
        gf2d_mouse_update();
    }
    if(mousePosition.y <= 0){
        slog("We hit the y 0");
        SDL_WarpMouseInWindow(NULL, mousePosition.x, 719);
        gf2d_mouse_update();

    }
    if(mousePosition.y >= 719){
        slog("We hit the Y max");
        SDL_WarpMouseInWindow(NULL, mousePosition.x, 1);
        gf2d_mouse_update();
    }
    else if(gf2d_mouse_moved()){
        mouseMovement = gf2d_mouse_get_movement();

        if(fabs(mouseMovement.x) > 1000){mouseMovement.x = 1;}
        if(fabs(mouseMovement.y) > 500){mouseMovement.y = 1;}

        mouseMovement.x =  smoothedVel.x * (1 - SMOOTH_FACTOR) + mouseMovement.x * SMOOTH_FACTOR;
        smoothedVel.x = mouseMovement.x;
        mouseMovement.y =  smoothedVel.y * (1 - SMOOTH_FACTOR) + mouseMovement.y * SMOOTH_FACTOR;
        smoothedVel.y = mouseMovement.y;
        mouseMovement.x = floor(mouseMovement.x);
        mouseMovement.y = floor(mouseMovement.y);
        slog("Mouse Move: X: %f Y: %f", mouseMovement.x, mouseMovement.y);        

        angle -= mouseMovement.x / 100;
        vangle += mouseMovement.y / 10; 
    }
    else{
        //WINDOW X
        //if(mousePosition.x >= 1280){SDL_WarpMouseInWindow(NULL, 0, mousePosition.y);}

        //SDL_WarpMouseInWindow(NULL, 0, 0);
    }
    //slog("X: %f Y: %f", mousePosition.x, mousePosition.y);
    data->angle = angle;
    //dampen? is it fine till we get our mouse setup?
    gfc_vector3d_rotate_about_z(&offset, angle);

    //Up and down: part of vangle
    
    /* if(gfc_input_command_down("panup")){
        vangle += vpanStep;
    }
    if(gfc_input_command_down("pandown")){
        vangle -= vpanStep;
    }*/
    if(vangle < -12.25){vangle = -12.25;}
    if(vangle > 30.75){vangle = 30.75;}
    data->vangle = vangle;

   
    //gfc_vector3d_rotate_about_x(&offset, vangle);

    gfc_vector3d_scale(offset, offset,followDist);  
    gfc_vector3d_add(self->position, data->target->position,offset);

    //TODO make it so it cant go below a certain point
    
    self->position.z = vangle + data->target->position.z;         
    //slog("%f, %f, %f: %f",self->position.x, self->position.y, self->position.z, vangle);
    gf3d_camera_look_at(data->target->position, &self->position);
    //Is is subtract to get the forward vector?
    gfc_vector3d_sub(data->forward, self->position, data->target->position);
    gfc_vector3d_normalize(&data->forward);
    //slog("FORWARD: %f %f %f", data->forward.x, data->forward.y, data->forward.z);

}
/*
void camera_entity_free(CameraEntityData *self){
   
   
    //very strange, is it calling itself or something :think: look out for that

    if(!self){return;}
    if(self->free)self->free(self);
    //We actually dont own the target, so idk what else we would free
    memset(self,0,sizeof(CameraEntityData));
    
}*/

Entity *camera_entity_spawn(GFC_Vector3D position, Entity *target){
    CameraEntityData *data;
    //GFC_Vector3D dir;
    Entity *self;
    self = entity_new();
    if(!self)return NULL;

    data = gfc_allocate_array(sizeof(CameraEntityData),1);
    self->data = data;
    self->position = position;
    self->think = camera_entity_think;
    //self->free = camera_entity_free;
    data->target = target;
    //15 and 8 originally
    data->followDist = 40;
    data->followHeight = 13;
    data->angle = GFC_PI;
    //set the data tartget as target
    //subtract to get the unit vector in front of our face?
    //gfc_vector3d_sub()
    smoothedVel = gfc_vector2d(0,0);
    return self;

}
