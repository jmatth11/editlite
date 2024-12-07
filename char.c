#include "char.h"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_START_RANGE 33
#define CHAR_END_RANGE 127

int init_char(struct characters *ch, const struct win *w, const char* ttf_file) {
  ch->font = TTF_OpenFont(ttf_file, ch->size);
  if (ch->font == NULL) {
    printf("TTF font could not initialize.\n");
    exit(1);
  }
  char glyph_buf[2];
  glyph_buf[1] = '\0';
  for (int i = CHAR_START_RANGE; i < CHAR_END_RANGE; ++i) {
    glyph_buf[0] = i;
    SDL_Surface *s = TTF_RenderText_Solid(ch->font, glyph_buf, ch->color);
    if (s == NULL) {
      printf("rendering TTF surface failed. %s\n", SDL_GetError());
    }
    SDL_Texture *new_glyph = SDL_CreateTextureFromSurface(w->renderer, s);
    if (new_glyph == NULL) {
      printf("error with glyph: %s\n", SDL_GetError());
    } else {
      ch->glyphs[i - CHAR_START_RANGE] = new_glyph;
    }
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
    SDL_Texture *tmp = ch->glyphs[i];
    if (tmp != NULL) {
      SDL_DestroyTexture(tmp);
    }
  }
  TTF_CloseFont(ch->font);
}
