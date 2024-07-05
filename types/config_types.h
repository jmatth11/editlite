#ifndef EDITLITE_CONFIG_TYPES_H
#define EDITLITE_CONFIG_TYPES_H

#include <SDL2/SDL_pixels.h>
#include <stddef.h>
#include "../deps/array_template/array_template.h"

#define EDITLITE_PLUGIN_PATH ".config/editlite\0"
#define CONFIG_FILENAME ".editlite.toml\0"

generate_array_template(string, char*);

struct config {
  double font_scale;
  double fps;
  size_t font_point;
  SDL_Color font_color;
  SDL_Color cursor_color;
  int win_height;
  int win_width;
  size_t read_size;
  char leader;
  char *font_file;
  char *plugin_path;
  char *home_path;
  string_array plugins;
};

#endif
