#include "gf2d_sprite.h"
#include "dialogue.h"
#include "monster.h"
#include "gf2d_mouse.h"
#include "gfc_input.h"
#include "dino.h"
typedef struct {
    Sprite          *sprite_list;
    Uint32          sprite_count;
    GFC_TextBlock   *current_words;
    Uint8           visible;
    SJson           *current_dialogue; 
    int             index;
} DialogueManager;
static DialogueManager dialogue_manager = {0};
void dialogue_end();

void dialogue_init(){

    dialogue_manager.sprite_list = (Sprite *)gfc_allocate_array(sizeof(Sprite),10);
    dialogue_manager.sprite_count = 10;
    //slog("we are dia logue");

    //0 is the textbox
    dialogue_manager.sprite_list[0] = *gf2d_sprite_load_image("images/dialogue.png");
    dialogue_manager.sprite_list[1] = *gf2d_sprite_load_image("models/dino/agudino.png");
    dialogue_manager.sprite_list[2] = *gf2d_sprite_load_image("models/dino/dino.png");

    dialogue_manager.current_words = "";
    dialogue_manager.index = 0;
}

int flag = 0;
void dialogue_think(Entity *self){
    //We do the dialogue
    //AKA did we click space bar
    //if we did then iterate the index of thing and the next draw call itll be the next line
    //We need a conversation list? and read them from def files?
    //sj_object_get_vector3d(dialogue_manager.current_dialogue,\, &pos);
    //int count, i;
    //    count = sj_array_get_count(dialogue_manager.current_dialogue);
    //    for (i = 0; i < count; i++) {

    //    }
    SJson *dialogue_json = dialogue_manager.current_dialogue;
    SJson *text_array = sj_object_get_value(dialogue_json, "text");

    if (!text_array || text_array->sjtype != SJVT_Array) {
        return;
    }

    SJson *text = sj_array_get_nth(text_array, dialogue_manager.index);
    if (!text || text->sjtype != SJVT_String) {
        return;
    }

    char *words = sj_get_string_value(text);
    //slog("%s, %i", dialogue_manager.current_words, dialogue_manager.index);

    


// Proceed to the next dialogue line after a button press or event

    if(gf2d_mouse_button_pressed(0)){
        dialogue_manager.current_words = words;
        dialogue_manager.index++;
        dino_sprite_next();
    }

    if(gfc_input_command_down("yes") && gfc_stricmp(dialogue_manager.current_words, "Congrats") != 0){
        slog("We bought stuff");
        monster_buy_hat(flag);
        flag++;
        dialogue_manager.current_words = "Congrats";
    }
    else if (gfc_input_command_down("no")){
        dialogue_manager.current_words = "ENDL";
    }


    if(gfc_stricmp(dialogue_manager.current_words, "ENDL") == 0){
        //slog("Thats the ends %i", dialogue_manager.index);
        words = 0;
        dialogue_end();
        return;
    }
}
void dialogue_update(Entity *self){
    //We do the dialogue
}

void set_think_to_dialogue(int flag){
    Entity *self;
    self = player_get_the();
    if(self->think != dialogue_think)
    {
        dino_sprite_next();

        self->think = dialogue_think;
        self->update = dialogue_update;
        shop_dialogue_begin(flag);
        dialogue_manager.visible = true;
//        gf2d_mouse_show();

    }
    else{
        //self->think = monster_think;
        //self->update = monster_update;
    }
    
}

void draw_dialogue(){
    if(dialogue_manager.visible && dialogue_manager.current_words){
        GFC_Vector2D position;
        position = gfc_vector2d(0,0);
        //gfc_vector2d(120,450)
        gf2d_sprite_draw_image(&dialogue_manager.sprite_list[0], position);
        gf2d_font_draw_line_tag(dialogue_manager.current_words,4,GFC_COLOR_BLACK, gfc_vector2d(120,500));

    }
}
void shop_dialogue_begin(int flag){
    //In theory we should read from a def but idgaf atp
    slog("WHAT");
    if(flag == 0){
    dialogue_manager.current_dialogue = sj_load("defs/shop_dialogue.def");
    }
    else{
        dialogue_manager.current_dialogue = sj_load("defs/shop_dialogue_2.def");
    }
    SJson *dialogue_json = dialogue_manager.current_dialogue;
    SJson *text_array = sj_object_get_value(dialogue_json, "text");

    if (!text_array || text_array->sjtype != SJVT_Array) {
        return;
    }

    SJson *text = sj_array_get_nth(text_array, dialogue_manager.index);
    if (!text || text->sjtype != SJVT_String) {
        return;
    }

    char *words = sj_get_string_value(text);
    dialogue_manager.current_words = words;
    dialogue_manager.index++;
    
}

void dialogue_end(){
    slog("ENDING!!");
    dialogue_manager.index = 0;
    dialogue_manager.visible = false;
    set_think_for_movement(player_get_the(), 3);
}