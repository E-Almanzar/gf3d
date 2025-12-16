#include "simple_logger.h"
#include "entity.h"
void dialogue_init();
void draw_dialogue();
void shop_dialogue_begin(int flag);
void set_think_to_dialogue(int flag);
void dialogue_think(Entity *self);