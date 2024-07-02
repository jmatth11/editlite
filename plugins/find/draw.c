#include "draw.h"
#include <SDL2/SDL_render.h>

void draw_background(struct display *d, int x, int y, int w, int h) {
  SDL_Rect box = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };
  SDL_SetRenderDrawColor(d->w.renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(d->w.renderer, &box);
}

void draw_textinput(struct display *d, const char *val, const int val_size,
                    int x, int y, int w, int h) {
  SDL_Rect box = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };
  SDL_SetRenderDrawColor(d->w.renderer, 0x45, 0x45, 0x45, 0xff);
  SDL_RenderFillRect(d->w.renderer, &box);
  int x_offset = x;
  int y_offset = y;
  const int char_w = d->glyphs.width;
  const int char_h = d->glyphs.height;
  SDL_Rect char_rect = {
    .x = x_offset,
    .y = y_offset,
    .w = char_w,
    .h = char_h,
  };
  for (int i = 0; i < val_size; ++i) {
    SDL_Texture *char_ren = d->texture_from_char(d, val[i]);
    if (char_ren == NULL) {
      char_ren = d->texture_from_char(d, '?');
    }
    SDL_RenderCopy(d->w.renderer, char_ren, NULL, &char_rect);
    char_rect.x += char_w;
  }
}
