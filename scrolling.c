#include "scrolling.h"
#include "linked_list.h"

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  const int c_row = d->cursor.pos.row;
  const size_t page_len = linked_list_get_len(cur_page->lines);
  if (c_row >= page_len) {
    d->cursor.pos.row = page_len - 1;
  } else if (c_row >= cur_page->row_offset + dim.row) {
    cur_page->row_offset++;
  } else if (c_row < cur_page->row_offset) {
    cur_page->row_offset = c_row;
  }
  if ((c_row+10) >= page_len &&
    cur_page->file_offset_pos < cur_page->file_size) {
    d->page_mgr.read(cur_page);
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  const int gap_buffer_len = gap_buffer_get_len(&cur_line->value.chars);
  if (d->cursor.pos.col >= gap_buffer_len) {
    d->cursor.pos.col = gap_buffer_len - 1;
  } else if (d->cursor.pos.col >= cur_page->col_offset + dim.col) {
    cur_page->col_offset++;
  } else if (d->cursor.pos.col < cur_page->col_offset) {
    cur_page->col_offset = d->cursor.pos.col;
  }
}

