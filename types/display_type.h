#ifndef EDITLITE_DISPLAY_TYPES_H
#define EDITLITE_DISPLAY_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include "state_types.h"

enum display_mode {
  NORMAL = 0,
  INSERT,
  COMMAND,
  PLUGIN_INPUT,
};

struct display {
  enum display_mode mode;
  struct app_state state;
  SDL_Texture *(*texture_from_char)(struct display *, const char);
};

struct character_display {
  // reserve 5 for future utf-8 work
  char buf[5];
  SDL_Texture *glyph;
  SDL_Rect display_pos;
  size_t row;
  size_t col;
};

#endif
