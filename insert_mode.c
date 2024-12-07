#include "insert_mode.h"
#include "display.h"
#include "gap_buffer.h"
#include "linked_list.h"
#include "page.h"
#include "scrolling.h"
#include <SDL2/SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>

void handle_insert_mode(struct display *d, struct win *w, SDL_Event *e) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    d->mode = NORMAL;
    return;
  }
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return;
  }
  struct display_dim dim;
  display_get_page_dim(d, w, &dim);

  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    cur_page->handle_backspace(cur_page, d);
  } else {
    cur_page->handle_keystroke(cur_page, d, e);
  }
  handle_row_scroll(d, dim);
  handle_col_scroll(d, dim);
}

void prepare_insert_mode(struct display *d, struct win *w, enum insert_mode_t mode) {
  d->mode = INSERT;
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for prepare_insert_mode.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  const size_t cur_gb_len = gap_buffer_get_len(cur_gb);
  switch (mode) {
    case INSERT_AT: {
      break;
    }
    case INSERT_BEGIN: {
      d->cursor.pos.col = 0;
      break;
    }
    case INSERT_END: {
      d->cursor.pos.col = cur_gb_len;
      break;
    }
    case INSERT_AFTER: {
      ++d->cursor.pos.col;
      break;
    }
  }
  if (d->cursor.pos.col >= cur_gb_len) {
    char tmp = ' ';
    gap_buffer_get_char(cur_gb, cur_gb_len - 1, &tmp);
    if (tmp == '\n') {
      d->cursor.pos.col = cur_gb_len - 1;
    } else {
      d->cursor.pos.col = cur_gb_len;
    }
  }
  gap_buffer_move_cursor(cur_gb, d->cursor.pos.col);
}
