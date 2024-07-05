#include <ctype.h>
#include <stdbool.h>
#include <SDL2/SDL_keycode.h>

#include "display.h"
#include "insert_mode.h"
#include "linked_list.h"
#include "types/display_type.h"
#include "scrolling.h"
#include "state.h"

void handle_insert_mode(struct display *d, SDL_Event *e) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    d->mode = NORMAL;
    return;
  }
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return;
  }
  struct display_dim dim;
  state_get_page_dim(&d->state, &dim);

  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    cur_page->handle_backspace(cur_page);
  } else {
    cur_page->handle_keystroke(cur_page, e);
  }
  handle_row_scroll(d, dim);
  handle_col_scroll(d, dim);
}

void prepare_insert_mode(struct display *d, enum insert_mode_t mode) {
  d->mode = INSERT;
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for prepare_insert_mode.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, cur_page->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  const size_t cur_gb_len = gap_buffer_get_len(cur_gb);
  switch (mode) {
    case INSERT_AT: {
      break;
    }
    case INSERT_BEGIN: {
      cur_page->cursor.pos.col = 0;
      break;
    }
    case INSERT_END: {
      cur_page->cursor.pos.col = cur_gb_len;
      break;
    }
    case INSERT_AFTER: {
      ++cur_page->cursor.pos.col;
      break;
    }
  }
  if (cur_page->cursor.pos.col >= cur_gb_len) {
    char tmp = ' ';
    gap_buffer_get_char(cur_gb, cur_gb_len - 1, &tmp);
    if (tmp == '\n') {
      cur_page->cursor.pos.col = cur_gb_len - 1;
    } else {
      cur_page->cursor.pos.col = cur_gb_len;
    }
  }
  gap_buffer_move_cursor(cur_gb, cur_page->cursor.pos.col);
}
