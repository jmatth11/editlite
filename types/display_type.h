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

#endif
