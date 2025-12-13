#include "gf2d_sprite.h"
#include "dialogue.h"

typedef struct {
    Sprite          *sprite_list;
    Uint32          sprite_count;
} DialogueManager;
static DialogueManager dialogue_manager = {0};

void dialogue_init(){

    dialogue_manager.sprite_list = (Sprite *)gfc_allocate_array(sizeof(Sprite),10);
    dialogue_manager.sprite_count = 10;
    //slog("we are dia logue");

    dialogue_manager.sprite_list[0] = *gf2d_sprite_load_image("models/dino/dino.png");
    dialogue_manager.sprite_list[1] = *gf2d_sprite_load_image("models/dino/agudino.png");
    dialogue_manager.sprite_list[2] = *gf2d_sprite_load_image("models/dino/dino.png");

}

void draw_dialogue(){
    GFC_Vector2D position;
    position = gfc_vector2d(0,0);
    //gf2d_sprite_draw_image(&dialogue_manager.sprite_list[0], position);
}