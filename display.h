#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include "char.h"
#include "page.h"

struct display {
  size_t cur_buf;
  struct characters glyphs;
  struct page_manager pages;
};

int init_display(struct display* d, const struct win *w);
int page_render(struct display *d, struct win *w);
void free_display(struct display *d);

#endif
