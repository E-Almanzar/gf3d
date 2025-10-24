#include "simple_logger.h"
#include "monster.h"
#include "gf3d_camera.h"
#include "entity.h"
#include "gfc_input.h"
#include "camera_entity.h"

typedef struct{
    Entity          *cam;
    GFC_Vector3D    forward;
}MonsterEntityData;

void monster_update(Entity *self){
    //You might have a billion memory leaks rn
    if(!self) return;
    //slog("Monster updating?");
    //slog("%f? %f", self->position.y, self->velocity.y);
    //We need to instead use the camera's x and move in that direction
    GFC_Vector3D camForward, newVelocity = {0}, right, forward, move = {0};
    GFC_Vector3D direction; 
    MonsterEntityData *data;
    data = malloc(sizeof(MonsterEntityData));
    data = self->data;
    Entity *cam;
    cam = data->cam; //Where is your forward, new thing in entity
    //if(!cam){//slog("NO CAM??");  
        //free(data); return;}
    CameraEntityData *data2;
    data2 = malloc(sizeof(CameraEntityData));
    data2 = cam->data;
    camForward = data2->forward;
    forward.x = camForward.x; forward.y = camForward.y; forward.z = 0;
    //slog("CAM POSITION: %f, %f, %f", cam->position.x,cam->position.y, cam->position.z);

    //If my forward is .8, .4 then you need to move it that much forward
    //With the forward you can calculate right too- cross product it?
    gfc_vector3d_cross_product(&right, forward, gfc_vector3d(0,0,1));
    if(self->velocity.x){
        //global right is 0,1,0
        if(self->velocity.x < 0){
            gfc_vector3d_add(self->position, self->position,right);
            //gfc_vector3d_add(data->forward, self->position,right);
            //newVelocity.x++;
            gfc_vector3d_add(move, move, forward);

        }
        else{
            gfc_vector3d_sub(self->position, self->position,right);
            //gfc_vector3d_sub(data->forward, self->position,right);
            //newVelocity.x--;
            gfc_vector3d_sub(move, move, forward);

        }
        newVelocity.x = self->velocity.x * right.x;
        
    }
    if(self->velocity.y){
        //global right is 0,1,0
        if(self->velocity.y < 0){
           gfc_vector3d_add(self->position, self->position,forward);
           //gfc_vector3d_add(data->forward, self->position,forward);
            //newVelocity.y++;
            gfc_vector3d_sub(move, move, right);


        }
        else{
            gfc_vector3d_sub(self->position, self->position,forward);
            //gfc_vector3d_sub(data->forward, self->position,forward);
            //newVelocity.y--;
            gfc_vector3d_add(move, move, right);


        }
        newVelocity.y = self->velocity.y * forward.y;

        //data->forward.y = self->position.y;
    }
    //self->position.y += self->velocity.y;  
    //self->position.z += self->velocity.z;
    //self->position.x += self->velocity.x;  
  
    //No clue how to get it to turn where we want anymore
    if((self->velocity.x) || (self->velocity.y)){
        self->rotation.z = atan2(move.y, move.x);
        //self->rotation.z = gfc_vector2d_angle(gfc_vector2d(newVelocity.x, newVelocity.y));
        //self->rotation.z = gfc_vector2d_angle(gfc_vector2d(data->forward.x, data->forward.y));
        slog("wrotation? %f %f %f", self->rotation.z, self->velocity.x, self->velocity.y);
    }

}

//velocity will just be x speed y speed and z speed
void monster_think(Entity *self){
    
    //GFC_Vector3D cameraDir;
    //MonsterEntityData *data;
    //CameraEntityData *data2;
    float move = 0;
    float moveStep = .1;
    //slog("self not data yet");
    if((!self)||(!self->data))return;
    //data = malloc(sizeof(MonsterEntityData));
    //data = self->data;
    //data2 = malloc(sizeof(CameraEntityData));
    //data2 = data->cam->data;
    //just use self->data loser
    //slog("self.data");
    //Cameras forward has to be our forward- add it in that direction?



    if(gfc_input_command_down("walkforward")){
        //slog("Movein forward");
        move += moveStep;
    }
        if(gfc_input_command_down("walkback")){
        //slog("Movein forward");
        move -= moveStep;
    }
    self->velocity.y = move;
    move = 0;
    if(gfc_input_command_down("walkleft")){
        //slog("Movein forward");
        move -= moveStep;
    }
        if(gfc_input_command_down("walkright")){
        //slog("Movein forward");
        move += moveStep;
    }
    self->velocity.x = move;
    move = 0;
    if(gfc_input_command_down("jump")){
        move += moveStep;
    }
    self->velocity.z = move;

    //float angle = data2->angle;
    //slog("WHAT %f", angle);
    //gfc_vector3d_add(self->position, data->cam->position,offset);






   /* if(!self) return;
     
    //self->position.x += self->speed;
    //self->position.x += self->velocity.x;
    //slog("Monster thinking?");

    self->rotation.z += self->speed;
    //self->rotation.x += self->speed;
    //self->rotation.y += self->speed;
    //self->rotation.z += .1;
    if(fabs(self->position.x) > 150){
        //self->velocity.x *= -1;
        self->position.x *=-1;
        self->speed *=-1;
    }
    //slog("x position: %.2f; x velocity: %.2f", self->position.x, self->velocity.x);*/
}

void monster_set_cam(Entity* self, Entity *cam){
    //if not, return
    if((!self)||(!cam)){ slog("no self or cam in set cam"); return; }
    MonsterEntityData *data;

    //Where does self->data come from?
    if((!self)||(!self->data))return;
    //slog("yes self or data in set cam");
    data = malloc(sizeof(MonsterEntityData));
    data->cam = cam;
    self->data = data;
    //slog("Monster set cam");

    //Very strange... so no more segfault but instead all black :think:

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
    
    return self;
}

void monster_free(){
    //Copy from camera

}


/*eol@eof*/