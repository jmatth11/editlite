#include "components/display.h"
#include "structures/linked_list.h"
#include "scrolling.h"
#include "states/state.h"
#include "types/display_type.h"
#include "types/unicode_types.h"

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_row_scroll.\n");
    return;
  }
  const size_t page_len = linked_list_get_len(cur_page->lines);
  if (cur_page->cursor.pos.row < 0) {
    cur_page->cursor.pos.row = 0;
  }
  if (cur_page->file_offset_pos >= cur_page->file_size &&
    cur_page->cursor.pos.row > (page_len - 1)) {
    cur_page->cursor.pos.row = page_len - 1;
  }
  const size_t c_row = cur_page->cursor.pos.row;
  if (cur_page->fp != NULL && (c_row+10) >= page_len &&
    cur_page->file_offset_pos < cur_page->file_size) {
    size_t new_page_len = page_len;
    struct linked_list *cur_end = linked_list_get_end(cur_page->lines);
    // keep reading until we get to where the user's cursor wants to go
    while ((c_row+10) >= new_page_len) {
      d->state.page_mgr.read(cur_page, d->state.config.read_size);
      new_page_len += linked_list_get_len(cur_end);
      cur_end = linked_list_get_end(cur_end);
    }
  }
  if (c_row >= cur_page->page_offset.row + dim.row) {
    const int offset = c_row - (cur_page->page_offset.row + dim.row);
    cur_page->page_offset.row += offset + 1;
  } else if (c_row < cur_page->page_offset.row) {
    cur_page->page_offset.row = c_row;
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_col_scroll.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines,
                                                     cur_page->cursor.pos.row);
  const int gap_buffer_len = gap_buffer_get_len(&cur_line->value.chars);
  if (cur_page->cursor.pos.col < 0) cur_page->cursor.pos.col = 0;
  if (cur_page->cursor.pos.col >= gap_buffer_len) {
    code_point_t tmp = ' ';
    gap_buffer_get_char(&cur_line->value.chars, gap_buffer_len - 1, &tmp);
    if (tmp == '\n') {
      cur_page->cursor.pos.col = gap_buffer_len - 1;
    } else {
      cur_page->cursor.pos.col = gap_buffer_len;
    }
  }
  const size_t c_col = cur_page->cursor.pos.col;
  if (c_col >= cur_page->page_offset.col + dim.col) {
    const int offset = c_col - (cur_page->page_offset.col + dim.col);
    cur_page->page_offset.col += offset + 1;
  } else if (c_col < cur_page->page_offset.col) {
    cur_page->page_offset.col = c_col;
  }
}

