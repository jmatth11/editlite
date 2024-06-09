#include "config.h"

void init_config(struct config *c) {
  c->leader = '\\';
  c->font_size = 14;
  c->cursor_color = (SDL_Color){0xAA,0xAA,0xAA,0x77};
  c->font_color = (SDL_Color){0xFF,0xFF,0xFF,0xFF};
  c->font_file = "resources/RobotoMono-Regular.ttf";
}
