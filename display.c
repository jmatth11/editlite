#include <SDL2/SDL_render.h>
#include "display.h"
#include "char.h"
#include "page.h"
#include "util.h"

#define CHARACTER_PADDING 0

int init_display(struct display* d, const struct win *w) {
  d->cursor.pos = (struct display_dim){0, 0};
  d->cursor.screen_pos = d->cursor.pos;
  d->cur_buf = 0;
  d->glyphs.color = d->config.font_color;
  d->glyphs.size = d->config.font_size;
  int err = init_char(&d->glyphs, w, d->config.font_file);
  if (err != 0) return err;
  err = init_page_manager(&d->page_mgr);
  if (err != 0) return err;
  return 0;
}

SDL_Texture * handle_characters(struct display *d, const char cur_char) {
  if (cur_char == '\t') {
    return get_glyph(&d->glyphs, ' ');
  }
  return get_glyph(&d->glyphs, cur_char);
}

void reset_cursor_screen_pos(struct display *d) {
  d->cursor.screen_pos = d->cursor.pos;
}

void draw_cursor(struct display *d, struct win *w, SDL_Rect *rect) {
  const SDL_Color c = d->config.cursor_color;
  SDL_SetRenderDrawColor(w->renderer, c.r, c.g, c.b, c.a);
  SDL_RenderFillRect(w->renderer, rect);
}

int page_render(struct display *d, struct win *w) {
  if (d->page_mgr.pages.len < 1) {
    return 0;
  }
  struct page cur_page = d->page_mgr.pages.page_data[d->cur_buf];
  struct display_dim dims;
  get_page_dim(d, w, &dims);
  int width_offset=0;
  int height_offset=0;
  reset_cursor_screen_pos(d);
  const int line_start = cur_page.row_offset;
  const int line_end = MIN(cur_page.lines.len, dims.row + cur_page.row_offset);
  for (int line_idx = line_start; line_idx < line_end; ++line_idx) {
    const struct line tmp_line = cur_page.lines.line_data[line_idx];
    const int char_start = cur_page.col_offset;
    const int char_len = MIN(tmp_line.chars.len, dims.col + cur_page.col_offset);
    if (d->cursor.pos.row == line_idx && d->cursor.pos.col >= char_len) {
      d->cursor.screen_pos.col = char_len - 1;
    }
    for (int char_idx = char_start; char_idx < char_len; ++char_idx) {
      char cur_char = tmp_line.chars.char_data[char_idx];
      SDL_Texture *glyph = handle_characters(d, cur_char);
      SDL_Rect r = {
        .x = width_offset,
        .y = height_offset,
        .w = d->glyphs.max_width,
        .h = d->glyphs.max_height,
      };
      if (glyph == NULL) {
        // ignore newline character
        if (cur_char != 10) {
          printf("cur_char: %d\n", cur_char);
          printf("glyph was null\n");
        } else if (d->cursor.screen_pos.row == line_idx && d->cursor.screen_pos.col == char_idx ) {
          draw_cursor(d, w, &r);
        }
        continue;
      }
      if (d->cursor.screen_pos.row == line_idx && d->cursor.screen_pos.col == char_idx) {
        draw_cursor(d, w, &r);
      }
      SDL_RenderCopy(
        w->renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += d->glyphs.max_width + CHARACTER_PADDING;
    }
    width_offset = 0;
    height_offset += d->glyphs.max_height + CHARACTER_PADDING;
  }
  return 0;
}

void get_page_dim(struct display *d, struct win *w, struct display_dim *out) {
  int winh, winw;
  SDL_GetWindowSize(w->window, &winw, &winh);
  out->row = (int)winh / d->glyphs.max_height;
  out->col = ((int)winw / d->glyphs.max_width);
}

void free_display(struct display* d) {
  free_char(&d->glyphs);
  free_page_manager(&d->page_mgr);
}
