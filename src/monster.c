#include "simple_logger.h"
#include "monster.h"
#include "gf3d_camera.h"
#include "entity.h"
#include "gfc_input.h"
#include "camera_entity.h"
#include "world.h"
#define GRAVITY -.4
#define JUMP 10
typedef struct{
    Entity              *cam;
    CameraEntityData    *camData;
    GFC_Vector3D        forward;
    Uint8               jumpAllowed;
}MonsterEntityData; //Padding?

MonsterEntityData *MonsterData;

//1 for allowed to jump, 0 for not
Uint8 jumpAllowed = 0;
Uint8 jumping = 0;


/*
    Return -1 for "you can do another step safely"
    otherwise return the distance that they can move in the next step

    TODO move into entity.c
*/
float validate_move_between(float velocity, float entpos, float targetpos, Entity *self){
    if(!velocity || !entpos || !targetpos){
        slog("Nothing in validate move");
        return 0;
    }
    float distance = entpos-targetpos;
    if(fabs(distance) > fabs(velocity)){
        //slog("difference, velocity %f, >  %f, %i", fabs(distance), fabs(velocity), fabs(distance) > fabs(velocity));
        return velocity;
    }

    /*
        If the difference is less than or equal to velocity- theres a problem
        calculate the allowed distance

        do we set the v velocity to zero here?
        
    */
   self->velocity.z = 0;
   //slog("Distance: %f", distance);
   if(velocity < 0)
       return -1*distance;
   return distance; 
}


void monster_gravity(Entity *self){
    GFC_Vector3D *contact;
    int hitFloor;
    float moveValid;
    contact = malloc(sizeof(GFC_Vector3D));
    hitFloor = entity_get_floor_position(self, world_get_the(), contact);



    if(!hitFloor){slog("Didnt hit the floor??");return;}
    
    
    self->velocity.z += GRAVITY;
    /*if(self->position.z - contact->z <= .025){
        moveValid = 0;
        self->velocity.z = 0;
        self->position.z = contact->z;
    }*/
    //else{
        moveValid = validate_move_between(self->velocity.z, self->position.z, contact->z,self);
    //}

    //slog("MOVEVALID?: %f", moveValid);
    if(fabs(moveValid) >= .001){
        self->position.z += moveValid;
    }
    else {//if (fabs(self->velocity.z-self->position.z) <= .001){
        //This is wrong, it stops at the peak
        jumpAllowed = 1;
        //jumping = 0;
        //slog("STEADY????????????????????????????????");
        //Weve hit the ground already and have no more velocities
    }
    //slog("~~~~~~~~~~~~~~!%f!~~~~~~~~~~~~~~", self->position.z);

/*
    //float vmove = self->velocity.z;
    if(self->velocity.z > -3){
        self->velocity.z = self->velocity.z +  GRAVITY;
    }
    
    contact = malloc(sizeof(GFC_Vector3D));
    hitFloor = entity_get_floor_position(self, world_get_the(), contact);
    //self->velocity.z = vmove;
    if(!hitFloor) return;

    slog("Velocity: %f, Position: %f Contact: %f",self->velocity.z, self->position.z, contact->z);
    
    if(self->position.z > contact->z){
        //slog("IS %f > %f????", self->position.z, contact->z);
        //downward
        self->position.z += self->velocity.z;
    }
    else{// if(self->position.z - contact->z < .5 || self->position.z < contact->z){
        //On the ground- let us jump
        //We will subdivide each step and check if its valid
        
        validate_move_between(self->velocity.z, self->position.z, contact->z);
        
        if(self->velocity.z >0){
            self->position.z += self->velocity.z;

        }else
        {
            self->position.z = contact->z;
            jumpAllowed = 1;
       }
        
        //snap to floor?
        //slog("HELLO?");
        //slog("IS %f < %f????", self->position.z, contact->z);
        //Is this the only place where floor contact will be made?

    }

    if(self->velocity.z > 0){
        //self->position.z += self->velocity.z;
    }
    /
    //slog("gravity");
    if(hitFloor){
        //slog("hit the floor");
        //slog("%i, %f, %f, %f",hitFloor, contact->x, contact->y, contact->z);
        //slog("POSITION: %i, %f, %f, %f",hitFloor, self->position.x, self->position.y, self->position.z);
        slog("Velocity: %f, Position: %f",self->velocity.z, self->position.z);
        if(self->position.z > contact->z){
            //slog("IS %f > %f????", self->position.z, contact->z);
            self->position.z += self->velocity.z;
        }
//        gfc_vector3d_copy(self->position, gfc_vector3d(contact->x, contact->y, contact->z));
    }
    if(self->velocity.z)
    {
        slog("Jump was pressed- add an impulse upward");
        //You move up or down based on vertical velocity
        if(self->position.z > contact->z){
            if(self->velocity.z > GRAVITY)
                self->velocity.z -= .01;
        }
        self->position.z += self->velocity.z;
        //We now have the floor below you
    }/
*/
}
void monster_move(Entity *self){
    GFC_Vector3D forward, camForward, right, move = {0};
    //Fix for bounce?
    float pVelocity = self->velocity.z;

    camForward = MonsterData->camData->forward;
    
    forward.x = camForward.x; forward.y = camForward.y; forward.z = 0;

    gfc_vector3d_cross_product(&right, forward, gfc_vector3d(0,0,1));
    if(self->velocity.x){
        if(self->velocity.x < 0){
            gfc_vector3d_add(self->position, self->position,right);
            gfc_vector3d_add(move, move, forward);

        }
        else{
            gfc_vector3d_sub(self->position, self->position,right);
            gfc_vector3d_sub(move, move, forward);

        }
    }
    if(self->velocity.y){
        if(self->velocity.y < 0){
           gfc_vector3d_add(self->position, self->position,forward);
            gfc_vector3d_sub(move, move, right);


        }
        else{
            gfc_vector3d_sub(self->position, self->position,forward);
            gfc_vector3d_add(move, move, right);
        }
    }

    if((self->velocity.x) || (self->velocity.y)){
        self->rotation.z = atan2(move.y, move.x);
    }

    //TODO- Jump w/ gravity
    monster_gravity(self);

    slog(" jumpallowed: %i, pVelocity:%f ",jumpAllowed, pVelocity);
    if(jumpAllowed && pVelocity>0){
        //slog("Both?");
        self->position.z += pVelocity;
    }
    else{
        //slog("JumpAllowed: %i, Jumpin %i,vel: %f", jumpAllowed, jumping, self->velocity.z);
    }
}
/*
    IMPORTANT- Update
    We want to do no decisions here, only obtain information and change states
*/
void monster_update(Entity *self){
    if(!self) return;
    monster_move(self);
    


}
void monster_control(Entity *self){
   float move = 0;
    float moveStep = .1;
    if((!self)||(!self->data))return;


    /*Input*/
    if(gfc_input_command_down("walkforward")){
        move += moveStep;
    }
        if(gfc_input_command_down("walkback")){
        move -= moveStep;
    }
    self->velocity.y = move;
    move = 0;
    if(gfc_input_command_down("walkleft")){
        move -= moveStep;
    }
        if(gfc_input_command_down("walkright")){
        move += moveStep;
    }
    self->velocity.x = move;
    //Jump
    if(jumpAllowed){
        move = self->velocity.z;
        if(gfc_input_command_down("jump")){
            
            //jumping = 1;
            jumpAllowed = 0;
            //slog("jumpin? %i", jumping);
            
            //if(self->position.z < JUMP){
                self->velocity.z += JUMP;
            //}
        }
    }
}

/*
    IMPORTANT- Think, make the decisions here, do not change states
*/
void monster_think(Entity *self){
    if(!self){return;}
    
    //contact = malloc(sizeof(GFC_Vector3D));
    monster_control(self);
    
    //We now have a very dirty contact (ik its not ideal but who cares)
    //Now we need to not set the Z in entity, and instead do it here, and make it stop

    //slog("got it: %i", x);

}
//This has to run AFTER monster data init
//This should fix the very messyness
void monster_set_cam(Entity* self, Entity *cam){
    if((!self)||(!cam)){ slog("no self or cam in set cam"); return; }
    if((!self->data))return;
    CameraEntityData *cdata;
    MonsterData->cam = cam;
    cdata = malloc(sizeof(CameraEntityData));
    cdata = cam->data;
    MonsterData->camData = cdata;
}

void snap_to_ground(Entity *self){
    if(!self)return;
    GFC_Vector3D *contact;
    int didCont;
    contact = malloc(sizeof(GFC_Vector3D));
    
    didCont = entity_get_floor_position(self, world_get_the(), contact);
    slog("%i, %f, %f, %f",didCont, contact->x, contact->y, contact->z);
    //gfc_vector3d_copy(self->position, gfc_vector3d(contact->x, contact->y, contact->z+4.91));
    free(contact);
}

void monster_data_init(Entity *self){
    MonsterEntityData *data;
    data = malloc(sizeof(MonsterEntityData));
    MonsterData = data;
}
Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color){
    Entity *self;
    self = entity_new();
    if(!self) return NULL;
    /*
    self->mesh = gf3d_mesh_load("models/dino/dino.obj");
    self->texture = gf3d_texture_load("models/dino/dino.png");
    strcpy(self->mesh->filename, "models/dino/dino.obj");
    */
    //DATA = GFC_ALLOCATE from camera
    //self->data , self->free 
    self->mesh = gf3d_mesh_load("models/alien/alien.obj");
    self->texture = gf3d_texture_load("models/alien/alien.png");
    strcpy(self->mesh->filename, "models/alien/alien.obj");

    self->color = Color;
    self->position = position;
    self->think = monster_think;
    self->data = gfc_allocate_array(sizeof(MonsterEntityData), 1);
    //self->free is something

    self->update = monster_update;
    //self->velocity = position;
    // = gfc_vector3d(.25,0,0);
    
    //self->velocity.z = .25;

    //self->velocity.x = .25;
    //slog("WHAT %f", self->velocity.x);
    self->speed = gfc_random()/10;
    //slog("WHAT %i", self->speed);
    self->rotation.z = 3.141592;
    
    //slog("Creating %s", self->mesh->filename);
//Somewhere you need to pass the name!!!!!!111
    /*slog("checking everything mesh: %i, texture: %i, postion: %i",
        self->mesh != 0,
        self->texture != 0,
        self->mesh != 0
    );*/
    
    
    //I guess we should spawn it on the world?
    monster_data_init(self);
    //Its just gonna be hardcoded?
    //snap_to_ground(self);
    
    return self;
}

void monster_free(){
    //Copy from camera

}


/*eol@eof*/