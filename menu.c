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
  if (init_menu_item_array(&m->items, 1) != 0) return false;
  return true;
}

void draw_select(struct display *d, struct win *w, SDL_Rect *rect) {
  const SDL_Color c = d->config.cursor_color;
  SDL_SetRenderDrawColor(w->renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(w->renderer, rect);
}

bool menu_display(struct display *d, struct win *w) {
  // TODO would love to clean up a lot of these variables/calulations
  int win_h, win_w;
  SDL_GetWindowSize(w->window, &win_w, &win_h);
  const int total_row_pixel_height = d->menu.items.len * d->glyphs.max_height;
  const int width = win_w - 50;
  const int height = MIN(total_row_pixel_height, win_h - 20);
  const int x_offset = (win_w / 2) - (width / 2);
  const int y_offset = (win_h /2) - (height / 2);
  const int rows_showing = ((double)height / (double)total_row_pixel_height) * d->menu.items.len;
  int row_offset = 0;
  if (rows_showing != d->menu.items.len && d->menu.idx >= rows_showing - 1) {
    row_offset = MIN((d->menu.idx - (rows_showing - 1)), (d->menu.items.len - 1));
  }
  SDL_Rect r = {
    .x = x_offset,
    .y = y_offset,
    .h = height,
    .w = width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  size_t item_len = MIN(d->menu.items.len, row_offset + rows_showing);
  int width_offset = x_offset;
  int height_offset = y_offset;
  for (size_t i = row_offset; i < item_len; ++i) {
    r = (SDL_Rect){
      .x = width_offset,
      .y = height_offset,
      .w = width,
      .h = d->glyphs.max_height,
    };
    if (d->menu.idx == i) {
      draw_select(d, w, &r);
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
        .w = d->glyphs.max_width,
        .h = d->glyphs.max_height,
      };
      SDL_RenderCopy(
        w->renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += d->glyphs.max_width;
    }
    height_offset += d->glyphs.max_height;
    width_offset = x_offset;
  }
  return true;
}

void menu_free(struct menu *m) {
  free_menu_item_array(&m->items);
}
