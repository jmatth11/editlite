#ifndef EDITLITE_CONFIG_H
#define EDITLITE_CONFIG_H

#include <SDL2/SDL_pixels.h>
#include <stddef.h>
#include "deps/array_template/array_template.h"

#define CONFIG_FILENAME "editlite.toml"

generate_array_template(string, char*);

struct config {
  double font_scale;
  size_t font_point;
  SDL_Color font_color;
  SDL_Color cursor_color;
  int win_height;
  int win_width;
  char leader;
  char *font_file;
  string_array plugins;
};

void config_init(struct config *c);
void parse_config(struct config *c);

#endif
