#ifndef EDITLITE_CONFIG_H
#define EDITLITE_CONFIG_H

#include <SDL2/SDL_pixels.h>
#include <stddef.h>
#include "deps/array_template/array_template.h"

#define CONFIG_FILENAME ".editlite"

generate_array_template(string, char*);

struct config {
  size_t font_size;
  SDL_Color font_color;
  SDL_Color cursor_color;
  char leader;
  char *font_file;
  string_array plugins;
};

void config_init(struct config *c);
void parse_config(struct config *c);

#endif
