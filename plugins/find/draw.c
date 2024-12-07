#include <SDL2/SDL_render.h>
#include <types/display_type.h>

#include "draw.h"
#include "find.h"

void draw_background(struct display *d, int x, int y, int w, int h) {
  SDL_Rect box = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };
  SDL_SetRenderDrawColor(d->state.w.renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(d->state.w.renderer, &box);
}

void draw_textinput(struct display *d, const char *val, const int val_size,
                    int x, int y, int w, int h) {
  SDL_Rect box = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };
  SDL_SetRenderDrawColor(d->state.w.renderer, 0x45, 0x45, 0x45, 0xff);
  SDL_RenderFillRect(d->state.w.renderer, &box);
  int x_offset = x;
  int y_offset = y;
  const int char_w = d->state.glyphs.scaled_size.width;
  const int char_h = d->state.glyphs.scaled_size.height;
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
    SDL_RenderCopy(d->state.w.renderer, char_ren, NULL, &char_rect);
    char_rect.x += char_w;
  }
}

void draw_options(struct display *d, struct find_info *op, size_t len,
                  int x, int y, int w, int h) {

  int x_offset = x;
  int y_offset = y;
  const int char_w = d->state.glyphs.scaled_size.width;
  const int char_h = d->state.glyphs.scaled_size.height;
  SDL_Rect char_rect = {
    .x = x_offset,
    .y = y_offset,
    .w = char_w,
    .h = char_h,
  };
  int options_len = op->visual_offset + len;
  if (options_len > op->locs.len) options_len = op->locs.len;
  for (int i = op->visual_offset; i < options_len; ++i) {
    struct find_loc *loc = &op->locs.location_data[i];
    for (int j =0; j<loc->preview_size; ++j) {
      char cur_char = loc->preview[j];
      // skip special chars
      if (cur_char == '\n' || cur_char == '\t') continue;
      SDL_Texture *char_ren = d->texture_from_char(d, cur_char);
      if (char_ren == NULL) {
        char_ren = d->texture_from_char(d, '?');
      }
      SDL_RenderCopy(d->state.w.renderer, char_ren, NULL, &char_rect);
      char_rect.x += char_w;
    }
    char_rect.x = x_offset;
    char_rect.y += char_h;
  }
}

void draw_select_box(struct display *d, int x, int y, int w, int h) {
  SDL_Rect r = {
    .x = x,
    .y = y,
    .w = w,
    .h = h,
  };
  SDL_SetRenderDrawColor(d->state.w.renderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderDrawRect(d->state.w.renderer, &r);
}
