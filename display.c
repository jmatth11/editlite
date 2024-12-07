#include "display.h"
#include "char.h"
#include "page.h"

int init_display(struct display* d, const struct win *w) {
  d->cur_buf = 0;
  int err = init_char(&d->glyphs, w, "resources/SourceCodePro-Regular");
  if (err != 0) return err;
  err = init_page_manager(&d->pages);
  if (err != 0) return err;
  return 0;
}

int page_render(struct display *page, struct win *w) {
  struct page cur_page = page->pages.buf.page_data[page->cur_buf];

  return 0;
}
