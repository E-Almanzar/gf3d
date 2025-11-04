#include "simple_logger.h"
#include "monster.h"
#include "gf3d_camera.h"
#include "entity.h"
#include "gfc_input.h"
#include "camera_entity.h"
#include "world.h"
#include "monster.h"
#include "monster_thinks.h"
#include "game.h"

long long roll_timer = -100;

Uint8 timer_check(long long * previous_time, float how_long_until_again){
    slog("checking %lld - %lld = %lld >= %f", get_timer(), *previous_time, get_timer() - *previous_time, how_long_until_again);
    if(get_timer() - *previous_time >=  how_long_until_again){
        *previous_time = get_timer();
        return 1;    
    }
    return 0;
}

void bounce_think(Entity *self){
/*    
    self->velocity.x += .10;
    self->velocity.y += .10;
    self->velocity.z += .10;
*/
    //Ok so it takes the negative value
    self->velocity.z = fabs(self->velocity.z * 1.1 + .1); 
    //gfc_vector3d_add(self->velocity, self->velocity, MonsterData->forward);
    if(self->velocity.z > 10){
        self->think = monster_think;
        self->update = monster_update;
    }
    //slog("velocity.x: %f, velocity.y: %f, velocity.z: %f", self->velocity.x, self->velocity.y, self->velocity.z);
}

void bounce_update(Entity *self){

    if (!self)
        return;

    GFC_Vector3D forward, camForward, right, move = {0}, mHoriz, mForBack, up;

    camForward = get_data_from_player()->camData->forward;

    forward.x = camForward.x;
    forward.y = camForward.y;
    forward.z = 0;

    up = gfc_vector3d(0, 0, 1);
    gfc_vector3d_cross_product(&right, forward, up);
    gfc_vector3d_scale(mHoriz, right, self->velocity.x);
    gfc_vector3d_scale(mForBack, forward, self->velocity.y);
    gfc_vector3d_scale(up, up, self->velocity.z);

    if (self->velocity.x)
    {
        gfc_vector3d_sub(self->position, self->position, mHoriz);
        if (self->velocity.x < 0)
            gfc_vector3d_add(move, move, forward);
        else
            gfc_vector3d_sub(move, move, forward);
    }
    if (self->velocity.y)
    {
        gfc_vector3d_sub(self->position, self->position, mForBack);
        if (self->velocity.y < 0)
            gfc_vector3d_sub(move, move, right);
        else
            gfc_vector3d_add(move, move, right);
    }
    if (self->velocity.z)
    {
        gfc_vector3d_add(self->position, self->position, up);

    }

    if ((self->velocity.x) || (self->velocity.y))
    {
        self->rotation.z = atan2(move.y, move.x);
    }
    get_data_from_player()->forward = forward;

    self->bounds->x = self->position.x;
    self->bounds->y = self->position.y;
    self->bounds->z = self->position.z;
}



void teleport_update(Entity *self){

    if (!self)
        return;

    GFC_Vector3D forward, camForward, up;//,right, move = {0}, mHoriz, mForBack, up;

    camForward = get_data_from_player()->camData->forward;

    forward.x = camForward.x;
    forward.y = camForward.y;
    forward.z = 0;
    up = gfc_vector3d(0, 0, 1);
    gfc_vector3d_scale(up, up, self->velocity.z);
    if (self->velocity.z)
    {
        gfc_vector3d_add(self->position, self->position, up);
    }
    get_data_from_player()->forward = forward;

    self->bounds->x = self->position.x;
    self->bounds->y = self->position.y;
    self->bounds->z = self->position.z;
}

void teleport_think(Entity *self){
    


    //Ok so it takes the negative value
    //slog("V1:%f", self->velocity.z);
    if(self->velocity.z < 0){self->velocity.z *=-1;}
    //slog("V:%f", self->velocity.z);
    self->velocity.z += .075;
    self->rotation.z +=.1;
    //gfc_vector3d_add(self->velocity, self->velocity, MonsterData->forward);
    if(self->velocity.z > 5){
        self->think = monster_think;
        self->update = monster_update;
    }
    //slog("velocity.x: %f, velocity.y: %f, velocity.z: %f", self->velocity.x, self->velocity.y, self->velocity.z);
}


//Roll think?
void roll_think(Entity *self){

    GFC_Vector3D forward;
    forward = get_data_from_player()->camData->forward;


    //If the time has been more than 100 frames, then you return to the og thinks
    slog("roll, %lld", roll_timer);
    if(timer_check(&roll_timer, 50)){
        slog("timer end");
        self->think = monster_think;
        self->update = monster_update;
    }
    //gfc_vector3d_add(self->velocity, self->velocity, forward);
    self->velocity.z = 0; 

}

void roll_update(Entity *self){
    GFC_Vector3D forward, right, forward_backward, horizontal, move = {0};
    forward = get_data_from_player()->camData->forward;
    
    gfc_vector3d_cross_product(&right, forward, gfc_vector3d(0, 0, 1));
    gfc_vector3d_scale(forward_backward, forward, self->velocity.y);
    gfc_vector3d_scale(horizontal, right, self->velocity.x);

    if (self->velocity.x){
        gfc_vector3d_sub(self->position, self->position, horizontal);
        if (self->velocity.x < 0)
            gfc_vector3d_add(move, move, forward);
        else
            gfc_vector3d_sub(move, move, forward);
    }
        if (self->velocity.y)
        {
            gfc_vector3d_sub(self->position, self->position, forward_backward);
            if (self->velocity.y < 0)
                gfc_vector3d_sub(move, move, right);
            else
                gfc_vector3d_add(move, move, right);
        }
}