#include <stdbool.h>
#include <SDL2/SDL_keycode.h>

#include "components/display.h"
#include "insert_mode.h"
#include "scribe.h"
#include "structures/linked_list.h"
#include "types/display_type.h"
#include "inputs/scrolling.h"
#include "states/state.h"
#include "types/unicode_types.h"
#include "components/glyph.h"

void handle_insert_mode(struct display *d, SDL_Event *e) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    display_set_mode(d, NORMAL);
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
    struct Edit event_op = {
      .event = SCRIBE_DELETE,
      .character = 0,
      .row = cur_page->cursor.pos.row,
      .col = cur_page->cursor.pos.col,
    };
    enum ScribeErrors result = scribe_write(&d->state.scribe, event_op);
    if (result != SCRIBE_SUCCESS) {
      fprintf(stderr, "error with scribe write for backspace: %d\n", result);
      return;
    }
  } else if (e->key.keysym.sym == SDLK_RETURN) {
    struct Edit event_op = {
      .event = SCRIBE_ADD,
      .character = SDLK_RETURN,
      .row = cur_page->cursor.pos.row,
      .col = cur_page->cursor.pos.col,
    };
    enum ScribeErrors result = scribe_write(&d->state.scribe, event_op);
    if (result != SCRIBE_SUCCESS) {
      fprintf(stderr, "error with scribe write for newline: %d\n", result);
      return;
    }
  }
  handle_row_scroll(d, dim);
  handle_col_scroll(d, dim);
}

void handle_input_mode(struct display *d, SDL_Event *e) {
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return;
  }
  struct display_dim dim;
  state_get_page_dim(&d->state, &dim);
  const code_point_t code = code_point_from_sdl_input(e);
  struct Edit event_op = {
    .event = SCRIBE_ADD,
    .character = code,
    .row = cur_page->cursor.pos.row,
    .col = cur_page->cursor.pos.col,
  };
  enum ScribeErrors result = scribe_write(&d->state.scribe, event_op);
  if (result != SCRIBE_SUCCESS) {
    fprintf(stderr, "error with scribe write for keystroke: %d\n", result);
    return;
  }
  handle_row_scroll(d, dim);
  handle_col_scroll(d, dim);
}

void prepare_insert_mode(struct display *d, enum insert_mode_t mode) {
  display_set_mode(d, INSERT);
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
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
    code_point_t tmp = ' ';
    gap_buffer_get_char(cur_gb, cur_gb_len - 1, &tmp);
    if (tmp == '\n') {
      cur_page->cursor.pos.col = cur_gb_len - 1;
    } else {
      cur_page->cursor.pos.col = cur_gb_len;
    }
  }
  gap_buffer_move_cursor(cur_gb, cur_page->cursor.pos.col);
}
