#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include "glyph.h"
#include "page.h"
#include "config.h"
#include <stdbool.h>

enum display_mode {
  NORMAL = 0,
  INSERT,
  COMMAND,
};

struct display_dim {
  int col;
  int row;
};

struct cursor {
  struct display_dim pos;
  struct display_dim screen_pos;
};

struct display {
  enum display_mode mode;
  size_t cur_buf;
  struct cursor cursor;
  struct glyphs glyphs;
  struct page_manager page_mgr;
  struct config config;
};

int display_init(struct display* d, const struct win *w);
bool display_page_render(struct display *d, struct win *w);
bool display_get_cur_page(struct display *d, struct page **out);
void display_get_page_dim(struct display *d, struct win *w, struct display_dim *out);
void display_free(struct display *d);

#endif
