#include <SDL2/SDL_render.h>
#include "display.h"
#include "char.h"
#include "page.h"
#include "util.h"

#define CHARACTER_PADDING 1

int init_display(struct display* d, const struct win *w) {
  d->cur_buf = 0;
  d->glyphs.color = (SDL_Color){ 255, 255, 255, 255 };
  d->glyphs.size = 16;
  int err = init_char(&d->glyphs, w, "resources/SourceCodePro-Regular.ttf");
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

int page_render(struct display *d, struct win *w) {
  if (d->page_mgr.pages.len < 1) {
    return 0;
  }
  struct page cur_page = d->page_mgr.pages.page_data[d->cur_buf];
  struct display_dim dims;
  get_page_dim(d, w, &dims);
  int width_offset=0;
  int height_offset=0;
  const int line_start = cur_page.row_offset;
  const int line_end = MIN(cur_page.lines.len, dims.row + cur_page.row_offset);
  for (int line_idx = line_start; line_idx < line_end; ++line_idx) {
    const struct line tmp_line = cur_page.lines.line_data[line_idx];
    const int char_start = cur_page.col_offset;
    const int char_len = MIN(tmp_line.chars.len, dims.col + cur_page.col_offset);
    for (int char_idx = char_start; char_idx < char_len; ++char_idx) {
      char cur_char = tmp_line.chars.char_data[char_idx];
      SDL_Texture *glyph = handle_characters(d, cur_char);
      if (glyph == NULL) {
        // ignore newline character
        if (cur_char != 10) {
          printf("cur_char: %d\n", cur_char);
          printf("glyph was null\n");
        } else if (d->cursor.dim.row == line_idx && d->cursor.dim.col == char_idx ) {
          SDL_Rect boxFill = {
            .x = width_offset,
            .y = height_offset,
            .w = d->glyphs.max_width,
            .h = d->glyphs.max_height,
          };
          SDL_SetRenderDrawColor(w->renderer, 0x77, 0x77, 0x77, 0x77);
          SDL_RenderFillRect(w->renderer, &boxFill);
        }
        continue;
      }
      int outw, outh;
      SDL_QueryTexture(glyph, NULL, NULL, &outw, &outh);
      SDL_Rect r = {
        .x = width_offset,
        .y = height_offset,
        .w = outw,
        .h = outh
      };
      if (d->cursor.dim.row == line_idx && d->cursor.dim.col == char_idx) {
        SDL_SetRenderDrawColor(w->renderer, 0x77, 0x77, 0x77, 0x77);
        SDL_RenderFillRect(w->renderer, &r);
      }
      SDL_RenderCopy(
        w->renderer,
        glyph,
        NULL,
        &r
      );
      width_offset += outw + CHARACTER_PADDING;
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
  // TODO investigate why this is 7 off from what I think
  out->col = ((int)winw / d->glyphs.max_width) - 7;
}

void free_display(struct display* d) {
  free_char(&d->glyphs);
  free_page_manager(&d->page_mgr);
}
