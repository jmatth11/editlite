#include "display.h"
#include "char.h"
#include "page.h"
#include <SDL2/SDL_render.h>

#define CHARACTER_PADDING 5

int init_display(struct display* d, const struct win *w) {
  d->cur_buf = 0;
  int err = init_char(&d->glyphs, w, "resources/SourceCodePro-Regular.ttf");
  if (err != 0) return err;
  err = init_page_manager(&d->pages);
  if (err != 0) return err;
  return 0;
}

int page_render(struct display *d, struct win *w) {
  struct page cur_page = d->pages.buf.page_data[d->cur_buf];
  const struct line tmp_line = cur_page.lines.line_data[0];
  int width_offset=0;
  int height_offset=0;
  int top_height = 0;
  for (int i = 0; i < tmp_line.buffer.len; ++i) {
    char cur_char = tmp_line.buffer.char_data[i];
    SDL_Texture *glyph = get_glyph(&d->glyphs, cur_char);
    int outw, outh;
    SDL_QueryTexture(glyph, NULL, NULL, &outw, &outh);
    SDL_Rect r = {
      .x = width_offset,
      .y = height_offset,
      .w = outw,
      .h = outh
    };
    SDL_RenderCopy(
      w->renderer,
      glyph,
      NULL,
      &r
    );
    width_offset += outw + CHARACTER_PADDING;
    if (outh > top_height) {
      top_height = outh;
    }
  }
  height_offset += top_height + CHARACTER_PADDING;
  return 0;
}

void free_display(struct display* d) {
  free_char(&d->glyphs);
  free_page_manager(&d->pages);
}
