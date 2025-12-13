#include "simple_logger.h"
#include "monster.h"
#include "gf3d_camera.h"
#include "entity.h"
#include "gfc_input.h"
#include "camera_entity.h"
#include "world.h"
#include "monster_thinks.h"
#include "rigidbody.h"
//Monster is in timeout until we can get physics without it

typedef struct player_anim{
    Mesh        **mesh_list;
    Texture     **texture_list;
    Uint16      iterator;
}player_anim;
void anim_init(Entity *self);
#define GRAVITY -.4
#define JUMP 6
#define MAX_ANIMS 3
long long lazy_timer = 0;
long long last_time_rolled = -500;

void teleport_think(Entity *self);
void teleport_update(Entity *self);

MonsterEntityData *MonsterData;

// 1 for allowed to jump, 0 for not
Uint8 jumpAllowed = 0;
Uint8 jumping = 0;
static Entity *player;
GFC_Vector3D lapsePos;
/*
    Return -1 for "you can do another step safely"
    otherwise return the distance that they can move in the next step

    TODO move into entity.c
*/

//Is this our wall cling
void monster_push_back(Entity* self){
    //slog("PUSHING BACK");
    self->velocity.x *= -1;
    self->velocity.y *= -1;
    /*if (gfc_input_command_down("jump"))
    {
        //slog("WALL JUMP");
                //gfc_vector3d_negate(self->velocity,MonsterData->forward);
                //self->velocity.z = JUMP;
                
                monster_move(self, 1);
                return;
    }*/
    monster_move(self, 1);
}
// How the hell do we do coyote time?
void monster_gravity(Entity *self)
{
    GFC_Vector3D *contact;
    int hitFloor, hitmplat = false;//, i;
    float moveValid;
    Entity *target;

    contact = malloc(sizeof(GFC_Vector3D));
    hitFloor = entity_get_floor_position(self, world_get_the(), contact);
    
    //Did we hit the moving platform too?
    //TODO look here- platform?
    target = entity_check_collide(self, 1);
    if(target){
        //We found a moving platform
        //Oh i think here is the issue with the moving platform
       
        //slog("here?");
        //Were gonna do the game design choice to let you jump below
        if(self->position.z >= target->position.z){
            //slog("%f %s target: %f %s", self->position.z, self->name, target->position.z, target->name);
            //slog("were higher? or does this not work idiot, you gotta collide to collide");
            hitFloor = true;
            contact->x = self->position.x;
            contact->y = self->position.y;
            //HOW DO I GET CONTACT.z properly???
            contact->z = target->position.z;
            
            self->position.x += target->velocity.x;
            //self->velocity.z = 0;
            //slog("Here? %f", self->velocity.z);
            /*if(self->position.z - target->bounds->z < 1){
                jumpAllowed = 1;
    
            }*/
           hitmplat = true;
            //  slog("hit mplat");
        }   
        else{
            //We are below the mplat
            self->velocity.z = 0;
        }


    }


    //slog("Did contact %f, %f, %f", contact->x, contact->y, contact->z);

    /*
        Floating? no ground 500-ish units below? no gravity, no jump.
        Basically we want to make sure the player doesn't fall into an infinite pit
    */
    if (!hitFloor)
    {
        //Can we stop movement before you hit it?

        //Push back function?
        monster_push_back(self);
        //slog("Didnt hit the floor??");
        return;
    }

    if(self->velocity.z > -4){
        self->velocity.z += GRAVITY;
    }
    //This is where the mplat breaks?
    //If we are on a mplat, we need to calculate it? or just drop the feature imo.
    //or just leave it idgaf 

    if(!hitmplat){
        moveValid = validate_move_between(self->velocity.z, self->position.z, contact->z, self);
    }else{

        self->velocity.z = 0;
        jumpAllowed = 1;
        //slog("We hit mplat so we can jump. Right? %i", jumpAllowed);
        return;

    }
    // Uint8 t = self->position.z == contact->z;
    // slog("%f, pos %f, con %f", t, self->position.z, contact->z);
    if (fabs(moveValid) >= .0001)
    {
        /*
            If you can move vertically, move vertically
            zero out our horizontal v for a second?
            When youre on the ground and you didnt jump dont fall just yet
            Techinally this is not a deliverable so we should just ignore it- move on
        */
        jumpAllowed = 0;
        
        //slog("fabs of movevalid %f ", fabs(moveValid));
        //So why does it hit here?
        //slog("what the juice?");
        //self->position.z += self->velocity.z;
        //self->position.z += moveValid;
        self->velocity.z = moveValid;
    }
    else if (self->position.z - contact->z < .01 )//|| hitmplat
    {
        // On the floor
        self->velocity.z = 0;
        jumpAllowed = 1;
    }
   //You can jump at peak- bad
   /*else{jumpAllowed = 1;}*/
    //slog("Velocity: %f", self->velocity.z);
}
float sprintHelpy = 0; 
int forwardposneg = 1, sidesideposneg = 1;
int previousDirectionX =1, previousDirectionY =1;
void monster_move(Entity *self, Uint8 calledByPushback)
{
    slog("start of monster_move");

    GFC_Vector3D forward, camForward, right, move = {0}, mHoriz, mForBack;
    // Fix for bounce?
    float pVelocity = self->velocity.z;

    camForward = MonsterData->camData->forward;

    forward.x = camForward.x;
    forward.y = camForward.y;
    forward.z = 0;


    gfc_vector3d_cross_product(&right, forward, gfc_vector3d(0, 0, 1));
    //slog("RIGHT?? %f, %f, %f", right.x, right.y, right.x);

    //If were sprinting we should scale it more
    //Oscillating between 3 and 2.99
    if (sprintHelpy > 0 && !MonsterData->sprinting) {
        sprintHelpy -= 0.01;
        if (sprintHelpy < 0) sprintHelpy = 0;   
    }
    else if (MonsterData->sprinting && sprintHelpy < 3) {
        sprintHelpy += 0.01;
        if (sprintHelpy > 3) sprintHelpy = 3;   
    }

    //slog("%f", sprintHelpy);
    //How do we get the 
    //variable = (condition) ? expressionTrue : expressionFalse;
    if (self->velocity.x != 0) {
        sidesideposneg = (self->velocity.x >= 0) ? 1 : -1;
    }

    if (self->velocity.y != 0) {
        forwardposneg = (self->velocity.y >= 0) ? 1 : -1;
    }

    // Apply sliding if not moving actively
    if (sprintHelpy > 0 && self->velocity.x == 0) {
        self->velocity.x = sprintHelpy * sidesideposneg;
    }

    if (sprintHelpy > 0 && self->velocity.y == 0) {
        self->velocity.y = sprintHelpy * forwardposneg;
    }

    gfc_vector3d_scale(mHoriz, right, self->velocity.x+sprintHelpy * sidesideposneg);
    gfc_vector3d_scale(mForBack, forward, self->velocity.y+sprintHelpy * forwardposneg);

  //  slog("RIGHT AFTER?? %f, %f, %f", mHoriz.x, mHoriz.y, mHoriz.x);
    //mHoriz = right;
    // forward = gfc_vector3d_multiply(forward, self->velocity);

    if (self->velocity.x)
    {
        gfc_vector3d_sub(self->position, self->position, mHoriz);
        if (self->velocity.x < 0)
            gfc_vector3d_add(move, move, forward);
        else
            gfc_vector3d_sub(move, move, forward);
        /*if (self->velocity.x < 0)
        {
            gfc_vector3d_add(self->position, self->position, right);
            gfc_vector3d_add(move, move, forward);
        }
        else
        {
            gfc_vector3d_sub(self->position, self->position, right);
            gfc_vector3d_sub(move, move, forward);
        }*/
    }
    if (self->velocity.y)
    {
        gfc_vector3d_sub(self->position, self->position, mForBack);
        if (self->velocity.y < 0)
            gfc_vector3d_sub(move, move, right);
        else
            gfc_vector3d_add(move, move, right);
        /*if (self->velocity.y < 0)
        {
            gfc_vector3d_add(self->position, self->position, forward);
            gfc_vector3d_sub(move, move, right);
        }
        else
        {
            gfc_vector3d_sub(self->position, self->position, forward);
            gfc_vector3d_add(move, move, right);
        }*/
    }
    if(calledByPushback != 1){
        //if(calledByPushback != 2){
            if ((self->velocity.x) || (self->velocity.y))
            {
                self->rotation.z = atan2(move.y, move.x);
            }
        //}
        // TODO- Jump w/ gravity
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GRAVITY
        //slog("Right before gravity %f", self->velocity.z);
        monster_gravity(self);
        //slog("Right after gravity %f", self->velocity.z);
        // This is the actual jump
        //slog("Bruh jumpallowed: %i, pVelocity:%f ",jumpAllowed, pVelocity);
        /*if (jumpAllowed && pVelocity > 0)
        {
            //Jump on mplat fix
            slog("Both?, vel: %f , Pvel: %f", self->velocity.z, pVelocity);
            self->velocity.z += 10;
            //self->velocity.z += pVelocity;
        }
        else
        {
            //slog("JumpAllowed: %i, Jumpin %i,vel: %f", jumpAllowed, jumping, self->velocity.z);
        }*/
            //self->velocity.z += .1;
            //Its set to zero in validate move between

            if (jumpAllowed && pVelocity > 0){
                self->velocity.z += JUMP;
            }
            //slog("self vel %f, %i", self->velocity.z, jumpAllowed);
        
    }

    //Set the previous
    //previousDirectionX = (self->velocity.x <= 0 || previousDirectionX == -1) ? 1 : -1;
    //previousDirectionY = (self->velocity.x >= 0) ? 1 : -1;
    slog("end of monster_move");
}
/*
    IMPORTANT- Update
    We want to do no decisions here, only obtain information and change states
*/
void monster_update(Entity *self)
{
    slog("update");
    if (!self)
        return;
    monster_move(self, 0);
    //slog("%f, %f", self->position.z, self->velocity.z);
    self->position.z += self->velocity.z;

    self->bounds->x = self->position.x-4;
    self->bounds->y = self->position.y-4;
    self->bounds->z = self->position.z-4;

    //Set the other guys things?
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = self->position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = self->position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = self->position.z;
    slog("self: %f, %f, %f", self->position.x, self->position.y, self->position.z);
   
}
void monster_control(Entity *self)
{
    //Turn off sprinting at the start
    MonsterData->sprinting = 0;
    float move = 0;
    float moveStep = 1;
    if ((!self) || (!self->data))
        return;

    /*Input*/
    
    //Lets say we roll first
    //We need to check the roll timer
    //We can kinda just compare against long long lazy timer
    //if(lazy_timer - last_time_rolled > 500){
        if(gfc_input_command_down("roll")){
            set_think_for_movement(self, 0);
            //last_time_rolled = get_timer();
            return;
        }   
    //}
    //sidestep? rainbow?
        if(gfc_input_command_down("rainbow")){
            set_think_for_movement(self, 1);
            return;
        }
        if(gfc_input_command_down("glide") && self->think != glide_think){//
            set_think_for_movement(self, 2);
            return;
        }

    if (gfc_input_command_down("walkforward"))
    {
        move += moveStep;
    }
    if (gfc_input_command_down("walkback"))
    {
        move -= moveStep;
    }
    self->velocity.y = move;
    move = 0;
    if (gfc_input_command_down("walkleft"))
    {
        move -= moveStep;
    }
    if (gfc_input_command_down("walkright"))
    {
        move += moveStep;
    }
    self->velocity.x = move;
    // Jump
    if (jumpAllowed)
    {
        move = self->velocity.z;
        if (gfc_input_command_down("jump"))//&& (self->velocity.z == 0 || self->velocity.z == JUMP )
        {
               //slog("MAN WTF");


            // jumping = 1;
            jumpAllowed = 0;
            //slog("jumpin? %lf", self->velocity.z);

            // if(self->position.z < JUMP){
            self->velocity.z = JUMP;
            //}
        }
    }

    //SPRINT- if the button is pressed add a little bit
    //if its not, subtract until the velocity isnt 
    if(gfc_input_command_down("sprint")){//Sprinting
        //slog("sprintingting");
        MonsterData->sprinting = 1;// Where do we unset it?
    }

}

/*
    IMPORTANT- Think, make the decisions here, do not change states
*/
void monster_think(Entity *self)
{
    slog("we?");

    //lazy_timer++;
    if (!self)
    {
        return;
    }
    //slog("velocity.x: %f, velocity.y: %f, velocity.z: %f", self->velocity.x, self->velocity.y, self->velocity.z);
    // contact = malloc(sizeof(GFC_Vector3D));
    monster_control(self);
    self->velocity.x *= self->speed;
    self->velocity.y *= self->speed;
    //We can kinda hardcode in our small enchant
    if(self->scale.z == .1f && self->velocity.z < 0){
        self->velocity.z *= .75f;
    }

    // We now have a very dirty contact (ik its not ideal but who cares)
    // Now we need to not set the Z in entity, and instead do it here, and make it stop

    // slog("got it: %i", x);

    if (self->position.x - lapsePos.x > 1 || self->position.x - lapsePos.x < -1)
    {
        // slog("Lapsepos: %f, %f, %f", self->position.x, self->position.y, self->position.z);
        lapsePos = self->position;
    }

    //ANIM
    if(((struct player_anim*)self->anim_data)->iterator < MAX_ANIMS){
        ((struct player_anim*)self->anim_data)->iterator++;
    }
    else{
        ((struct player_anim*)self->anim_data)->iterator = 0;
    }
    
    //self->mesh = ((struct player_anim*)self->anim_data)->mesh_list[((struct player_anim*)self->anim_data)->iterator];
}
// This has to run AFTER monster data init
// This should fix the very messyness
void monster_set_cam(Entity *self, Entity *cam)
{
    if ((!self) || (!cam))
    {
        slog("no self or cam in set cam");
        return;
    }
    if ((!self->data))
        return;
    CameraEntityData *cdata;
    MonsterData->cam = cam;
    cdata = malloc(sizeof(CameraEntityData));
    cdata = cam->data;
    MonsterData->camData = cdata;
}

//Dead code
/*
void snap_to_ground(Entity *self)
{
    /if (!self)
        return;
    GFC_Vector3D *contact;
    int didCont;
    contact = malloc(sizeof(GFC_Vector3D));

    didCont = entity_get_floor_position(self, world_get_the(), contact);
    //slog("Did contact %i, %f, %f, %f", didCont, contact->x, contact->y, contact->z);
    // gfc_vector3d_copy(self->position, gfc_vector3d(contact->x, contact->y, contact->z+4.91));
    free(contact);/
}*/

void monster_data_init(Entity *self)
{
    MonsterEntityData *data;
    data = malloc(sizeof(MonsterEntityData));
    MonsterData = data;
    MonsterData->collected = 0;
}

Entity *monster_spawn(GFC_Vector3D position, GFC_Color Color)
{
    Entity *self;
    self = entity_new();
    if (!self)
        return NULL;
    /*
    self->mesh = gf3d_mesh_load("models/dino/dino.obj");
    self->texture = gf3d_texture_load("models/dino/dino.png");
    strcpy(self->mesh->filename, "models/dino/dino.obj");
    */
    // DATA = GFC_ALLOCATE from camera
    // self->data , self->free

    self->color = Color;
    self->position = position;
    self->think = monster_think;
    self->data = gfc_allocate_array(sizeof(MonsterEntityData), 1);
    // self->free is something

    self->update = monster_update;
    // self->velocity = position;
    //  = gfc_vector3d(.25,0,0);

    // self->velocity.z = .25;

    // self->velocity.x = .25;
    // slog("WHAT %f", self->velocity.x);
    self->speed = 1;
    // slog("WHAT %i", self->speed);
    self->rotation.z = 3.141592;

    // slog("Creating %s", self->mesh->filename);
    // Somewhere you need to pass the name!!!!!!111
    /*slog("checking everything mesh: %i, texture: %i, postion: %i",
        self->mesh != 0,
        self->texture != 0,
        self->mesh != 0
    );*/

    // I guess we should spawn it on the world?
    monster_data_init(self);

    // Box
    self->bounds = gfc_allocate_array(sizeof(GFC_Box), 1);
    self->bounds->x = position.x-4;
    self->bounds->y = position.y-4;
    self->bounds->z = position.z-4;
    self->bounds->w = 2;
    self->bounds->h = 2;
    self->bounds->d = 4;

    //slog("HOLY %f", self->rotation.y);
    strcpy(self->name, "Alien Guy");
    player = self;
    // float x,y,z;   //position of corner
    // float w,h,d;   // width, height, and depth offsets
    // Its just gonna be hardcoded?
    // snap_to_ground(self);
    lapsePos = self->position;

    //Rigidbody
        //Rigidbody
    self->s_bounds = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    self->s_bounds->x = position.x-5;
    self->s_bounds->y = position.y-5;
    self->s_bounds->z = position.z;
    self->s_bounds->r = 10;

    self->rigidbody_data = gfc_allocate_array(sizeof(Rigidbody), 1);
    ((struct Rigidbody*)self->rigidbody_data)->mass_inverse = 1;
    ((struct Rigidbody*)self->rigidbody_data)->bounciness = 1;
    ((struct Rigidbody*)self->rigidbody_data)->owner = self;
    gfc_vector3d_copy(((struct Rigidbody*)self->rigidbody_data)->position, self->position);
    
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere = gfc_allocate_array(sizeof(GFC_Sphere), 1);
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->r = 10;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->x = position.x;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->y = position.y;
    ((struct Rigidbody*)self->rigidbody_data)->rigid_sphere->z = position.z;
    ((struct Rigidbody*)self->rigidbody_data)->onFloor = 0;
    anim_init(self);
    physics_world_add(*(Rigidbody*)self->rigidbody_data);
    return self;
}
void anim_init(Entity *self){
    self->anim_data = gfc_allocate_array(sizeof(player_anim), 3);
    ((struct player_anim*)self->anim_data)->mesh_list = gfc_allocate_array(sizeof(Mesh), 3);
    ((struct player_anim*)self->anim_data)->mesh_list[0] = gf3d_mesh_load("models/alien/AlienwSaucer.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[1] = gf3d_mesh_load("models/alien/tiltL.obj");
    ((struct player_anim*)self->anim_data)->mesh_list[2] = gf3d_mesh_load("models/alien/tiltR.obj");

    self->mesh = ((struct player_anim*)self->anim_data)->mesh_list[0];
    self->texture = gf3d_texture_load("models/alien/Alien2.png");
    strcpy(self->mesh->filename, "models/alien/AlienwSaucer.obj");

}

void monster_free()
{
    // Copy from camera
}

// Maybe a function to expose the player?
// The design pattern will be selfish- you can check to see if you hit the player
// And if you did return 1, and then they can go get the player themselves
Entity *player_get_the()
{
    if (!player)
    {
        slog("no player in player get");
    }
    return player;
}

GFC_Vector3D player_get_forward(Entity *player)
{
    if (!player)
    {
        slog("no data in player data get");
    }
    return MonsterData->forward;
}

//These are for ONLY the jumppads and other world things that change the player state    
//Flag: 0- bounce 1- teleport
void set_think_to_bounce(Entity *self, Uint8 flag){
    //Flag: 0 is bounce
    //1 is teleport
    self->velocity.z = 1;
    //BOUNCE
    if(!flag){
        //slog("Changing %s's think to bounce", self->name);
        self->think = bounce_think;
        self->update = bounce_update;
        //self->velocity.z = 0;
        

    }
    //Teleport
    if(flag == 1){
        self->think = teleport_think;
        self->update = teleport_update;
        self->velocity.x = 0;
        self->velocity.y = 0;
        self->position.z += 15;


    }
}
void set_think_for_movement(Entity *self, Uint8 flag){
    //Roll
    if(!flag){
        //slog("we rolled");
        /*if(self->velocity.x == 0 && self->velocity.y == 0){
            slog("self vel %f %f Monster %f %f",self->velocity.x, self->velocity.y,  MonsterData->camData->forward.x,  MonsterData->camData->forward.y);
            self->velocity.x = MonsterData->camData->forward.x*-1;
            self->velocity.y = MonsterData->camData->forward.y*-1;
            
        }*/
       if(self->velocity.x == 0 && self->velocity.y == 0){return;}
        self->think = roll_think;
        self->update = roll_update;
    }
    //Rainbow Jump
    if(flag == 1){
        self->think = rainbow_think;
        self->update = rainbow_update;
        self->velocity.x = 0;
        self->velocity.z = 0;
    }

    if(flag == 2){
        self->think = glide_think;
        self->update = glide_update;
        //self->rotation.y = GFC_PI/.33;
    }
    
    if(flag == 3){
        self->think = monster_think;
        self->update = monster_update;
        self->rotation.y = 0;
        self->rotation.x = 0;
        //self->rotation.y = GFC_PI/.33;
    }

}

void set_think_to_dead(Entity *self){
    //slog("f u buddy");
    self->rotation.z = GFC_PI*2;
    self->think = dead_think;
    self->update = dead_update;

}


long long get_timer_from_player(){
    
    return lazy_timer;
}

MonsterEntityData * get_data_from_player(){
    return MonsterData;

}
void monster_collect(Entity *self, Entity *target){
    MonsterData->collected++;
}

int monster_get_collected(){
    return MonsterData->collected;
}

/*eol@eof*/