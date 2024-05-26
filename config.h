#ifndef EDITLITE_CONFIG_H
#define EDITLITE_CONFIG_H

#include <SDL2/SDL_pixels.h>
#include <stddef.h>

struct config {
  size_t font_size;
  SDL_Color font_color;
  SDL_Color cursor_color;
  char leader;
  char *font_file;
};

void init_config(struct config *c);

#endif
