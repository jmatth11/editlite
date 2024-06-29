#include "scrolling.h"
#include "linked_list.h"

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_row_scroll.\n");
    return;
  }
  const size_t page_len = linked_list_get_len(cur_page->lines);
  if (d->cursor.pos.row < 0) {
    d->cursor.pos.row = 0;
  }
  if (d->cursor.pos.row >= page_len) {
    d->cursor.pos.row = page_len - 1;
  }
  const size_t c_row = d->cursor.pos.row;
  if (c_row >= cur_page->row_offset + dim.row) {
    const int offset = c_row - (cur_page->row_offset + dim.row);
    cur_page->row_offset += offset + 1;
  } else if (c_row < cur_page->row_offset) {
    cur_page->row_offset = c_row;
  }
  if (cur_page->fp != NULL && (c_row+10) >= page_len &&
    cur_page->file_offset_pos < cur_page->file_size) {
    d->page_mgr.read(cur_page);
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_col_scroll.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  const int gap_buffer_len = gap_buffer_get_len(&cur_line->value.chars);
  if (d->cursor.pos.col < 0) d->cursor.pos.col = 0;
  if (d->cursor.pos.col >= gap_buffer_len) {
    char tmp = ' ';
    gap_buffer_get_char(&cur_line->value.chars, gap_buffer_len - 1, &tmp);
    if (tmp == '\n') {
      d->cursor.pos.col = gap_buffer_len - 1;
    } else {
      d->cursor.pos.col = gap_buffer_len;
    }
  }
  const size_t c_col = d->cursor.pos.col;
  if (c_col >= cur_page->col_offset + dim.col) {
    const int offset = c_col - (cur_page->col_offset + dim.col);
    cur_page->col_offset += offset + 1;
  } else if (c_col < cur_page->col_offset) {
    cur_page->col_offset = c_col;
  }
}

