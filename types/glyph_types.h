#ifndef EDITLITE_GLYPH_TYPES_H
#define EDITLITE_GLYPH_TYPES_H

#include "deps/array_template/array_template.h"
#include "size_types.h"
#include "types/unicode_types.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_ttf.h>

// TODO maybe allow caching range to be configurable
#define CHAR_START_RANGE 32
#define CHAR_END_RANGE 127

// forward declare
struct hash_map;

struct glyph_info {
  code_point_t point;
  SDL_Texture* glyph;
};

generate_array_template(glyph, struct glyph_info)

struct glyphs {
  double scale;
  SDL_Color color;
  size_t point;
  struct font_size unscaled_size;
  struct font_size scaled_size;
  TTF_Font *font;
  struct hash_map* glyphs;
  code_point_t (*sanitize_character)(code_point_t);
};


#endif
