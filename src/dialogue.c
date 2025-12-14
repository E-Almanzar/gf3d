#include "gf2d_sprite.h"
#include "dialogue.h"
#include "monster.h"
#include "gf2d_mouse.h"
#include "gfc_input.h"

typedef struct {
    Sprite          *sprite_list;
    Uint32          sprite_count;
    GFC_TextBlock   *current_words;
    Uint8           visible;
} DialogueManager;
static DialogueManager dialogue_manager = {0};

void dialogue_init(){

    dialogue_manager.sprite_list = (Sprite *)gfc_allocate_array(sizeof(Sprite),10);
    dialogue_manager.sprite_count = 10;
    //slog("we are dia logue");

    //0 is the textbox
    dialogue_manager.sprite_list[0] = *gf2d_sprite_load_image("images/dialogue.png");
    dialogue_manager.sprite_list[1] = *gf2d_sprite_load_image("models/dino/agudino.png");
    dialogue_manager.sprite_list[2] = *gf2d_sprite_load_image("models/dino/dino.png");

    dialogue_manager.current_words = "";
}

void dialogue_think(Entity *self){
    //We do the dialogue
    //AKA did we click space bar
    //if we did then iterate the index of thing and the next draw call itll be the next line
    //We need a conversation list? and read them from def files?
    if(gf2d_mouse_button_pressed(0)){
        dialogue_manager.current_words = "Holy moly guacamole cheese all holey easy peasy lemon squeezy";
    }
}
void dialogue_update(Entity *self){
    //We do the dialogue
}

void set_think_to_dialogue(){
    Entity *self;
    self = player_get_the();
    if(self->think != dialogue_think)
    {
        self->think = dialogue_think;
        self->update = dialogue_update;
        shop_dialogue_begin();
        dialogue_manager.visible = true;
    }
    else{
        //self->think = monster_think;
        //self->update = monster_update;
    }
}

void draw_dialogue(){
    if(dialogue_manager.visible){
    GFC_Vector2D position;
    position = gfc_vector2d(0,0);
    //gfc_vector2d(120,450)
    gf2d_sprite_draw_image(&dialogue_manager.sprite_list[0], position);
        gf2d_font_draw_line_tag(dialogue_manager.current_words,3,GFC_COLOR_BLACK, gfc_vector2d(120,500));

    }
}
void shop_dialogue_begin(){
    //In theory we should read from a def but idgaf atp
    dialogue_manager.current_words = "Heya pal";

    
}
