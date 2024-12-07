#include "scrolling.h"

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  const int c_row = d->cursor.pos.row;
  if (c_row >= cur_page->lines.len) {
    d->cursor.pos.row = cur_page->lines.len - 1;
  } else if (c_row >= cur_page->row_offset + dim.row) {
    cur_page->row_offset++;
  } else if (c_row < cur_page->row_offset) {
    cur_page->row_offset = c_row;
  }
  if ((c_row+10) >= cur_page->lines.len &&
    cur_page->file_offset_pos < cur_page->file_size) {
    d->page_mgr.read(cur_page);
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  const int gap_buffer_len = gap_buffer_get_len(&cur_line->chars);
  if (d->cursor.pos.col >= gap_buffer_len) {
    d->cursor.pos.col = gap_buffer_len - 1;
  } else if (d->cursor.pos.col >= cur_page->col_offset + dim.col) {
    cur_page->col_offset++;
  } else if (d->cursor.pos.col < cur_page->col_offset) {
    cur_page->col_offset = d->cursor.pos.col;
  }
}

