#include "command_prompt.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

bool command_prompt_handle_backspace(struct page *p, struct display *d) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, d->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (d->cursor.pos.col > 0) {
    gap_buffer_delete(cur_gb);
    d->cursor.pos.col--;
  }
  return true;
}

bool command_prompt_handle_keystroke(struct page *p, struct display *d, SDL_Event *e) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, d->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  const char received_char = sanitize_character(e->key.keysym.sym);
  if (received_char != '\0') {
    if (received_char == '\n') {
      // TODO handle executing commands
    } else {
      gap_buffer_insert(cur_gb, received_char);
      d->cursor.pos.col++;
    }
  }
  return true;
}

bool command_prompt_init(struct page *p) {
  if (!page_init(p)) return false;
  p->handle_backspace = command_prompt_handle_backspace;
  p->handle_keystroke = command_prompt_handle_keystroke;
  return true;
}

void command_prompt_free(struct page *p) {
  page_free(p);
}

bool command_prompt_display(struct display *d, struct win *w) {
  int win_h, win_w;
  SDL_GetWindowSize(w->window, &win_w, &win_h);
  SDL_Rect r = {
    .x = (win_w / 2) - (160 / 2),
    .y = (win_h /2) - 40,
    .h = 40,
    .w = 160,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  return 0;
}
