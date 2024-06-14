#ifndef EDITLITE_GLYPH_H
#define EDITLITE_GLYPH_H

#include <SDL2/SDL_ttf.h>
#include "win.h"

#define CHAR_START_RANGE 32
#define CHAR_END_RANGE 127

struct glyphs {
  TTF_Font *font;
  SDL_Texture* glyphs[CHAR_END_RANGE - CHAR_START_RANGE];
  SDL_Color color;
  size_t size;
  size_t max_height;
  size_t max_width;
};

int init_char(struct glyphs *ch, const struct win *w, const char* ttf_file);
SDL_Texture* get_glyph(struct glyphs *ch, const char c);
void free_char(struct glyphs *);

#endif