#include "command_prompt.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

bool command_prompt_handle_backspace(struct page *p, struct display *d) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, 0);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  if (d->cursor.pos.col > 0) {
    gap_buffer_delete(cur_gb);
    d->cursor.pos.col--;
  }
  return true;
}

bool command_prompt_handle_keystroke(struct page *p, struct display *d, SDL_Event *e) {
  struct linked_list *cur_line = linked_list_get_pos(p->lines, 0);
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
  const int width = win_w - 50;
  const int height = 60;
  d->command_prompt.x_offset = (win_w / 2) - (width / 2);
  d->command_prompt.y_offset = (win_h /2) - height;
  SDL_Rect r = {
    .x = d->command_prompt.x_offset,
    .y = d->command_prompt.y_offset,
    .h = height,
    .w = width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  return 0;
}
