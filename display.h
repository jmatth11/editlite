#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include "char.h"
#include "page.h"
#include "config.h"

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
  struct characters glyphs;
  struct page_manager page_mgr;
  struct config config;
};

int init_display(struct display* d, const struct win *w);
int page_render(struct display *d, struct win *w);
void get_page_dim(struct display *d, struct win *w, struct display_dim *out);
void free_display(struct display *d);

#endif
