#include "char.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#define CHAR_START_RANGE 33
#define CHAR_END_RANGE 176

int init_char(struct characters *ch, const struct win *w, const char* ttf_file) {
  ch->font = TTF_OpenFont(ttf_file, ch->size);
  for (int i = CHAR_START_RANGE; i <= CHAR_END_RANGE; ++i) {
    const char c = (char)i;
    SDL_Surface *s = TTF_RenderText_Solid(ch->font, &c, ch->color);
    ch->glyphs[i - CHAR_START_RANGE] = SDL_CreateTextureFromSurface(w->renderer, s);
    SDL_FreeSurface(s);
  }
  return 0;
}

SDL_Texture* get_glyph(struct characters *ch, const char c) {
  size_t idx = (int)c - CHAR_START_RANGE;
  return ch->glyphs[idx];
}

void free_char(struct characters *ch) {
  for (int i = 0; i < (CHAR_END_RANGE - CHAR_START_RANGE); ++i) {
    SDL_DestroyTexture(ch->glyphs[i]);
  }
  TTF_CloseFont(ch->font);
}
