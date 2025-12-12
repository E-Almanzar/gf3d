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
#include "rigidbody.h"

long long roll_timer = -100;
long long rainbow_timer = -100;

void set_think_for_ents(Entity *self){
    if(!self->name){
        return;
    }
    if(gfc_stricmp(self->name, "Alien Guy") == 0){
        self->think = monster_think;
        self->update = monster_update;
    }
    if(gfc_stricmp(self->name, "Ball") == 0){
        self->think = rigidbody_think;
        self->update = rigidbody_update;
    }
}

void monster_update_bounds(Entity *self){
    if(self->bounds){
        self->bounds->x = self->position.x;
        self->bounds->y = self->position.y;
        self->bounds->z = self->position.z;
    }
}


Uint8 timer_check(long long * previous_time, float how_long_until_again){
    //slog("checking %lld - %lld = %lld >= %f", get_timer(), *previous_time, get_timer() - *previous_time, how_long_until_again);
    if(get_timer() - *previous_time >=  how_long_until_again){
        //Are they racing???
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
    if(self->rigidbody_data && gfc_stricmp(self->name, "Alien Guy") != 0){
       self->velocity.z = ((struct Rigidbody*)self->rigidbody_data)->velocity.z;
    }
    self->velocity.z = fabs(self->velocity.z * 1.1 + .1); 
    //slog("bounce think %f", self->velocity.z);

    //gfc_vector3d_add(self->velocity, self->velocity, MonsterData->forward);
    if(self->velocity.z > 10){
        //self->think = monster_think;
        //self->update = monster_update;
        set_think_for_ents(self);
    }
    //slog("velocity.x: %f, velocity.y: %f, velocity.z: %f", self->velocity.x, self->velocity.y, self->velocity.z);
}

void bounce_update(Entity *self){
    if(gfc_stricmp(self->name, "Alien Guy") == 0){
        monster_update_bounds(self);
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
    }
    else{
        if(self->rigidbody_data){
            ((struct Rigidbody*)self->rigidbody_data)->velocity.x = self->velocity.x;
            ((struct Rigidbody*)self->rigidbody_data)->velocity.y = self->velocity.y;
            ((struct Rigidbody*)self->rigidbody_data)->velocity.z = self->velocity.z;
        }
    }

    if(self->bounds){
        self->bounds->x = self->position.x;
        self->bounds->y = self->position.y;
        self->bounds->z = self->position.z;
    }

}



void teleport_update(Entity *self){
    monster_update_bounds(self);
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
    
    //If the time has been more than 100 frames, then you return to the og thinks
    //slog("roll, %lld", roll_timer);
   // GFC_Vector3D forward;
   // forward = get_data_from_player()->camData->forward;

    if(timer_check(&roll_timer, 50)){
        //slog("timer end");
        self->think = monster_think;
        self->update = monster_update;
        self->rotation.y = 0;
        self->rotation.x = 0;
        //slog("END %f", self->rotation.y);
        return;
    }
    //gfc_vector3d_add(self->velocity, self->velocity, forward);
    self->rotation.y++;
    //slog("HOLY %f", self->rotation.y);


    self->velocity.z = 0; 

}

void roll_update(Entity *self){
    monster_update_bounds(self);
    GFC_Vector3D forward, right, forward_backward, horizontal, move = {0};
    forward = get_data_from_player()->camData->forward;
    forward.z = 0;
    slog("%f, %f, %f", forward.x, forward.y, forward.z);
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
        move.z = 0;
}

float rainbowdir = 1;

//Rainbow = sidestep left or right 
void rainbow_think(Entity *self){
    self->velocity.z += rainbowdir;
    self->velocity.x += 1;
    if(self->velocity.z >= 10 && rainbowdir >0){
        rainbowdir*=-1;
    }
    monster_move(self, 0);
    if(timer_check(&rainbow_timer, 50)){
        //slog("timer end %f", rainbowdir);
        self->think = monster_think;
        self->update = monster_update;
        //self->rotation.y = 0;
        rainbowdir = 1;
        //slog("END %f", self->rotation.y);
        return;
    }
 /*   GFC_Vector3D forward;
    forward = get_data_from_player()->camData->forward;


    //If the time has been more than 100 frames, then you return to the og thinks
    //slog("roll, %lld", roll_timer);
    if(timer_check(&rainbow_timer, 50)){
        slog("timer end %f", rainbowdir);
        self->think = monster_think;
        self->update = monster_update;
        //self->rotation.y = 0;
        //rainbowdir *= -1;
        //slog("END %f", self->rotation.y);
        return;
    }

    //if(self->velocity.y){
        //Then do the y routine
        //if youre at the peak, flip and go down
        //self->velocity.y += .4;
        self->velocity.x += rainbowdir;
        self->velocity.z += rainbowdir;
        if(self->velocity.z > 2 && rainbowdir > 0){
            rainbowdir *= -1;
        }
        slog("%f", self->velocity.z);

    //}

    //self->velocity.z = 0; 
*/
}

void rainbow_update(Entity *self){
    monster_update_bounds(self);
    /*GFC_Vector3D forward, right, forward_backward, horizontal, move = {0};
    forward = get_data_from_player()->camData->forward;
    slog("2: %f", self->velocity.z);

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
    //slog("3: %f", self->velocity.z);
    self->position.z += self->velocity.z;
    //monster_gravity(self);
    //slog("%f", self->velocity.z);
*/
}

//Glide think?
void glide_update(Entity *self){
        if (!self)
        return;
    monster_move(self, 2);
    
    self->position.z += self->velocity.z;

    self->bounds->x = self->position.x-4;
    self->bounds->y = self->position.y-4;
    self->bounds->z = self->position.z-4;

    //Set the other guys things?
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = self->position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = self->position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = self->position.z;
    //Set your rotation funky
        //self->rotation.z +=.01;
        self->rotation.z +=.01;
        self->rotation.y +=.01;

    monster_update_bounds(self);
}

void glide_think(Entity *self){
    GFC_Vector3D *contact;

    contact = malloc(sizeof(GFC_Vector3D));
    entity_get_floor_position(self, world_get_the(), contact);

    //|| gfc_input_command_down("glide") Can't turn it off just yet
    if(self->position.z - contact->z < .025){
        self->think = monster_think;
        self->update = monster_update;
        self->rotation.y = 0;
        self->rotation.x = 0;
        return;
    }
    /*if(gfc_input_command_down("c")){
        self->think = monster_think;
        self->update = monster_update;
    }*/
    monster_control(self);
    //self->rotation.z += .001;
//    self->rotation.x += .001;

    self->velocity.x *= 1.3;
    self->velocity.y *= 1.3;
    self->velocity.z *= .5;
    //We can kinda hardcode in our small enchant
    if(self->scale.z == .1f && self->velocity.z < 0){
        self->velocity.z *= .75f;
    }

}

void dead_update(Entity *self){
    monster_update_bounds(self);

}


void dead_think(Entity *self){
//If you hit respawn, respawn

    self->position = gfc_vector3d(0,0,100);
    get_data_from_player()->cam->position = gfc_vector3d(0,0,100); 
    //slog("dead as hell");
    if (gfc_input_command_down("jump"))
    {
        self->think = monster_think;
        self->update = monster_update;
        self->position = gfc_vector3d(0,0,100);
    }
    monster_update_bounds(self);
}

