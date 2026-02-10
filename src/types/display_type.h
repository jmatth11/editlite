#ifndef EDITLITE_DISPLAY_TYPES_H
#define EDITLITE_DISPLAY_TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include "state_types.h"
#include "types/page_types.h"
#include "types/unicode_types.h"

enum display_mode {
  NORMAL = 0,
  INSERT,
  COMMAND,
  PLUGIN_INSERT,
};

struct display {
  struct app_state state;
  SDL_Texture *(*texture_from_char)(struct display *, const code_point_t);
  struct viewable_page_info (*viewable_page_buffer)(struct display *, struct page *);
  void (*viewable_page_buffer_free)(struct viewable_page_info *);
  enum display_mode mode;
  char *plugin_mode_name;
  bool switching_mode;
};

struct character_display {
  code_point_t buf;
  SDL_Texture *glyph;
  SDL_Rect display_pos;
  size_t page_offset_row;
  size_t page_offset_col;
  size_t row;
  size_t col;
};

#endif
