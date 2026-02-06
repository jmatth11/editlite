#ifndef EDITLITE_GLYPH_H
#define EDITLITE_GLYPH_H

#include "types/unicode_types.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_keycode.h>

struct glyphs;
struct win;

int init_char(struct glyphs *ch, const struct win *w, const char* ttf_file);
SDL_Texture* get_glyph(struct glyphs *ch, const struct win *w, const code_point_t c);
void free_char(struct glyphs *);
code_point_t code_point_from_sdl_input(SDL_Event *);
code_point_t sanitize_character(code_point_t keycode);

#endif
