#include "char.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

const char *char_list = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789~`!@#$%^&*()_+-=,./<>?';:\"[]{}\\|";

int init_char(struct characters *ch, const struct win *w, const char* ttf_file) {
  const size_t n = strlen(char_list);
  ch->font = TTF_OpenFont(ttf_file, ch->size);
  for (int i = 0; i < n; ++i) {
    const char c = char_list[i];
    SDL_Surface *s = TTF_RenderText_Solid(ch->font, &c, ch->color);
    ch->glyphs[i] = SDL_CreateTextureFromSurface(w->renderer, s);
    SDL_FreeSurface(s);
  }
  return 0;
}

SDL_Texture* get_glyph(struct characters *ch, const char c) {
  char *pos = strchr(char_list, c);
  size_t idx = pos - char_list;
  return ch->glyphs[idx];
}

void free_char(struct characters *ch) {
  const size_t n = strlen(char_list);
  for (int i = 0; i < n; ++i) {
    SDL_DestroyTexture(ch->glyphs[i]);
  }
  TTF_CloseFont(ch->font);
}
