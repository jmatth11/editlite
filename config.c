#include "config.h"

void config_init(struct config *c) {
  c->leader = '\\';
  c->font_size = 14;
  c->cursor_color = (SDL_Color){0xAA,0xAA,0xAA,0x77};
  c->font_color = (SDL_Color){0xFF,0xFF,0xFF,0xFF};
  c->font_file = "resources/RobotoMono-Regular.ttf";
  if (init_string_array(&c->plugins, 1) != 0) {
    fprintf(stderr, "plugin array could not be initialized.\n");
  }
  insert_string_array(&c->plugins, "plugins/save/libsave.so");
  insert_string_array(&c->plugins, "plugins/quit/libquit.so");
}
