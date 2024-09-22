#ifndef EDITLITE_GLYPH_H
#define EDITLITE_GLYPH_H

#include "types/unicode_types.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_keycode.h>

struct glyphs;
struct win;

int init_char(struct glyphs *ch, const struct win *w, const char* ttf_file);
SDL_Texture* get_glyph(struct glyphs *ch, const struct win *w, const code_point_t c);
void free_char(struct glyphs *);
char sanitize_character(SDL_Keycode keycode);

#endif
