#include "insert_mode.h"
#include "page.h"

void handle_insert_mode(struct display *d, struct win *w, SDL_Event *e) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    d->mode = NORMAL;
    return;
  }
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  // TODO grab current line and start inserting/deleting characters
  // from line's gap buffer
}
