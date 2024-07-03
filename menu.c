#include "menu.h"
#include "display.h"
#include "win.h"
#include "util.h"

bool menu_init(struct menu *m) {
  m->w = 50;
  m->h = 60;
  m->x = 0;
  m->y = 0;
  m->idx = 0;
  m->visual_offset = 0;
  if (init_menu_item_array(&m->items, 1) != 0) return false;
  return true;
}

void draw_select(struct display *d, SDL_Rect *rect) {
  const SDL_Color c = d->config.cursor_color;
  SDL_SetRenderDrawColor(d->w.renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(d->w.renderer, rect);
}

bool menu_display(struct display *d) {
  // TODO would love to clean up a lot of these variables/calulations
  int win_h, win_w;
  SDL_GetWindowSize(d->w.window, &win_w, &win_h);
  const int total_row_pixel_height = d->menu.items.len * d->glyphs.height;
  const int width = win_w - 50;
  const int height = MIN(total_row_pixel_height, win_h - 20);
  const int x_offset = (win_w / 2) - (width / 2);
  const int y_offset = (win_h /2) - (height / 2);
  const int rows_showing = ((double)height / (double)total_row_pixel_height) * d->menu.items.len;
  if (rows_showing != d->menu.items.len &&
    d->menu.idx >= (d->menu.visual_offset + (rows_showing - 1))) {
    d->menu.visual_offset = d->menu.idx - (rows_showing - 1);
  }
  if (d->menu.idx < d->menu.visual_offset) {
    if (d->menu.idx == 0) {
      d->menu.visual_offset = 0;
    } else {
      d->menu.visual_offset -= (d->menu.visual_offset - d->menu.idx);
    }
  }
  SDL_Rect r = {
    .x = x_offset,
    .y = y_offset,
    .h = height,
    .w = width,
  };
  SDL_SetRenderDrawColor(d->w.renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(d->w.renderer, &r);
  size_t item_len = MIN(d->menu.items.len, d->menu.visual_offset + rows_showing);
  int width_offset = x_offset;
  int height_offset = y_offset;
  for (size_t i = d->menu.visual_offset; i < item_len; ++i) {
    r = (SDL_Rect){
      .x = width_offset,
      .y = height_offset,
      .w = width,
      .h = d->glyphs.height,
    };
    if (d->menu.idx == i) {
      draw_select(d, &r);
    }
    struct menu_item *item = &d->menu.items.menu_item_data[i];
    for (size_t char_idx = 0; char_idx < strlen(item->name); ++char_idx) {
      SDL_Texture *glyph = handle_characters(d, item->name[char_idx]);
      if (glyph == NULL) {
        glyph = handle_characters(d, '?');
      }
      r = (SDL_Rect){
        .x = width_offset,
        .y = height_offset,
        .w = d->glyphs.width,
        .h = d->glyphs.height,
      };
      SDL_RenderCopy(
        d->w.renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += d->glyphs.width;
    }
    height_offset += d->glyphs.height;
    width_offset = x_offset;
  }
  return true;
}

void menu_free(struct menu *m) {
  free_menu_item_array(&m->items);
}
