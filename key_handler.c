#include "key_handler.h"
#include "page.h"
#include "util.h"

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  if (d->cursor.dim.row >= cur_page->lines.len) {
    d->cursor.dim.row = cur_page->lines.len - 1;
  } else if (d->cursor.dim.row >= cur_page->row_offset + dim.row) {
    cur_page->row_offset++;
  } else if (d->cursor.dim.row < cur_page->row_offset) {
    cur_page->row_offset = d->cursor.dim.row;
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.dim.row];
  if (d->cursor.dim.col >= cur_line->chars.len) {
    d->cursor.dim.col = cur_line->chars.len - 1;
  } else if (d->cursor.dim.col >= cur_page->col_offset + dim.col) {
    cur_page->col_offset++;
  } else if (d->cursor.dim.col < cur_page->col_offset) {
    cur_page->col_offset = d->cursor.dim.col;
  }
}

void keydown_handler(struct display *d, struct win *w, SDL_Event *e) {
  struct display_dim dims;
  get_page_dim(d, w, &dims);
  switch (e->key.keysym.sym) {
    case SDLK_j:
      d->cursor.dim.row++;
      handle_row_scroll(d, dims);
      break;
    case SDLK_k:
      d->cursor.dim.row = MAX(0, d->cursor.dim.row - 1);
      handle_row_scroll(d, dims);
      break;
    case SDLK_l:
      d->cursor.dim.col++;
      handle_col_scroll(d, dims);
      break;
    case SDLK_h:
      d->cursor.dim.col = MAX(0, d->cursor.dim.col - 1);
      handle_col_scroll(d, dims);
      break;
  }
}
