#include "command_mode.h"

void handle_command_mode(struct display *d, struct win *w, SDL_Event *e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
  }
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_insert_mode.\n");
    return;
  }
  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    cur_page->handle_backspace(cur_page, d);
  } else {
    cur_page->handle_keystroke(cur_page, d, e);
  }
}

void prepare_command_mode(struct display *d, struct win *w) {
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for prepare_insert_mode.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, 0);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (cur_gb == NULL) fprintf(stderr, "command mode buffer is null\n");
  const size_t cur_gb_len = gap_buffer_get_len(cur_gb);
  gap_buffer_move_cursor(cur_gb, cur_gb_len - 1);
  gap_buffer_delete_seq(cur_gb, cur_gb_len - 1);
}
