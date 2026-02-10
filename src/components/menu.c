#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

#include "display.h"
#include "menu.h"
#include "types/menu_types.h"
#include "types/display_type.h"
#include "helpers/util.h"
#include "utf8.h"

bool menu_init(struct menu *m) {
  m->w = 50;
  m->h = 60;
  m->x = 0;
  m->y = 0;
  m->idx = 0;
  m->visual_offset = 0;
  if (!init_menu_item_array(&m->items, 1)) return false;
  return true;
}

static void inline draw_select(struct display *d, SDL_Rect *rect) {
  const SDL_Color c = d->state.config.cursor_color;
  SDL_SetRenderDrawColor(d->state.w.renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(d->state.w.renderer, rect);
}

static SDL_Rect inline generate_menu_rect(struct display *d, const int row_height) {
  int win_h, win_w;
  SDL_GetWindowSize(d->state.w.window, &win_w, &win_h);
  SDL_Rect menu_rect = {
    .x = 0,
    .y = 0,
    .w = win_w - 50,
    .h = MIN(row_height, win_h - 20),
  };
  menu_rect.x = (win_w / 2) - (menu_rect.w / 2);
  menu_rect.y = (win_h /2) - (menu_rect.h / 2);
  return menu_rect;
}

static int inline calculate_menu_visual_offsets(struct display *d, const int visual_height) {
  int result = d->state.menu.visual_offset;
  if (visual_height != d->state.menu.items.len &&
    d->state.menu.idx >= (d->state.menu.visual_offset + (visual_height - 1))) {
    result = d->state.menu.idx - (visual_height - 1);
  }
  if (d->state.menu.idx < d->state.menu.visual_offset) {
    if (d->state.menu.idx == 0) {
      result = 0;
    } else {
      result -= (
        d->state.menu.visual_offset - d->state.menu.idx
      );
    }
  }
  return result;
}

bool menu_display(struct display *d) {
  const int total_row_pixel_height = (
    d->state.menu.items.len * d->state.glyphs.scaled_size.height
  );
  const SDL_Rect menu_rect = generate_menu_rect(d, total_row_pixel_height);
  const int rows_showing = (
    ((double)menu_rect.h / (double)total_row_pixel_height) *
    d->state.menu.items.len
  );
  d->state.menu.visual_offset = calculate_menu_visual_offsets(d, rows_showing);
  SDL_Rect r = menu_rect;
  SDL_SetRenderDrawColor(d->state.w.renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(d->state.w.renderer, &r);
  size_t item_len = MIN(d->state.menu.items.len, d->state.menu.visual_offset + rows_showing);
  int width_offset = menu_rect.x;
  int height_offset = menu_rect.y;
  for (size_t i = d->state.menu.visual_offset; i < item_len; ++i) {
    r = (SDL_Rect){
      .x = width_offset,
      .y = height_offset,
      .w = menu_rect.w,
      .h = d->state.glyphs.scaled_size.height,
    };
    if (d->state.menu.idx == i) {
      draw_select(d, &r);
    }
    struct menu_item *item = &d->state.menu.items.menu_item_data[i];
    const size_t name_len = strlen(item->name);
    for (size_t char_idx = 0; char_idx < name_len;) {
      const struct code_point point = utf8_next((uint8_t *)item->name, name_len, char_idx);
      char_idx += octet_type_count(point.type);

      SDL_Texture *glyph = handle_characters(d, point.val);
      if (glyph == NULL) {
        glyph = handle_characters(d, '?');
      }
      r = (SDL_Rect){
        .x = width_offset,
        .y = height_offset,
        .w = d->state.glyphs.scaled_size.width,
        .h = d->state.glyphs.scaled_size.height,
      };
      bool mutated = false;
      if (item->use_color) {
        if (SDL_SetTextureColorMod(glyph, item->color.r, item->color.g, item->color.b) == -1) {
          fprintf(stderr, "color modulation not supported on this machine.\n");
        } else {
          mutated = true;
        }
      }
      SDL_RenderCopy(
        d->state.w.renderer,
        glyph,
        NULL,
        &r
      );
      if (mutated) {
        const SDL_Color orig_color = d->state.glyphs.color;
        SDL_SetTextureColorMod(glyph, orig_color.r, orig_color.g, orig_color.b);
      }
      width_offset += d->state.glyphs.scaled_size.width;
    }
    height_offset += d->state.glyphs.scaled_size.height;
    width_offset = menu_rect.x;
  }
  return true;
}

void menu_free(struct menu *m) {
  free_menu_item_array(&m->items);
}
