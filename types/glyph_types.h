#ifndef EDITLITE_GLYPH_TYPES_H
#define EDITLITE_GLYPH_TYPES_H

#include "size_types.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

#define CHAR_START_RANGE 32
#define CHAR_END_RANGE 127

struct glyphs {
  double scale;
  SDL_Color color;
  size_t point;
  struct font_size unscaled_size;
  struct font_size scaled_size;
  TTF_Font *font;
  SDL_Texture* glyphs[CHAR_END_RANGE - CHAR_START_RANGE];
  char (*sanitize_character)(SDL_Keycode);
};


#endif
