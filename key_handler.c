#include "key_handler.h"
#include "display.h"
#include "insert_mode.h"
#include "command_mode.h"
#include "linked_list.h"
#include "scrolling.h"
#include "page.h"
#include "util.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>

void handle_keydown(struct display *d, struct win *w, SDL_Event *e) {
  switch(d->mode) {
    case NORMAL: {
      handle_state_keypresses(d, w, e);
      break;
    }
    case INSERT:{
      handle_insert_mode(d, w, e);
      break;
    }
    case COMMAND:{
      handle_command_mode(d, w, e);
      break;
    }
    default:
    break;
  }
}

void handle_jump_commands(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  const int line_len = gap_buffer_get_len(&cur_line->value.chars) - 1;
  const int offset = line_len - dim.col;
  d->cursor.pos.col = line_len;
  cur_page->col_offset = MAX(offset, 0);
}

void handle_simple_keypresses(struct display *d, struct win *w, SDL_Event *e) {
  struct display_dim dims;
  display_get_page_dim(d, w, &dims);
  switch (e->key.keysym.sym) {
    case SDLK_j:
    case SDLK_DOWN:
      d->cursor.pos.row++;
      handle_row_scroll(d, dims);
      break;
    case SDLK_k:
    case SDLK_UP:
      d->cursor.pos.row = MAX(0, d->cursor.pos.row - 1);
      handle_row_scroll(d, dims);
      break;
    case SDLK_l:
    case SDLK_RIGHT:
      d->cursor.pos.col++;
      handle_col_scroll(d, dims);
      break;
    case SDLK_h:
    case SDLK_LEFT:
      if (d->cursor.pos.col != d->cursor.screen_pos.col) d->cursor.pos.col = d->cursor.screen_pos.col;
      d->cursor.pos.col = MAX(0, d->cursor.pos.col - 1);
      handle_col_scroll(d, dims);
      break;
    case SDLK_0:
      d->cursor.pos.col = 0;
      d->page_mgr.pages.page_data[d->cur_buf].col_offset = 0;
      break;
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
    case SDLK_i: {
      d->mode = INSERT;
      prepare_insert_mode(d, w);
      break;
    }
  }
}

void handle_state_keypresses(struct display *d, struct win *w, SDL_Event *e) {
  struct display_dim dims;
  display_get_page_dim(d, w, &dims);
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 lshift = key_states[SDL_SCANCODE_LSHIFT];
  const Uint8 rshift =key_states[SDL_SCANCODE_RSHIFT];
  const Uint8 k4 = key_states[SDL_SCANCODE_4];
  const Uint8 colon = key_states[SDL_SCANCODE_SEMICOLON];
  if (!lshift && !rshift) {
    handle_simple_keypresses(d, w, e);
  }
  if (lshift || rshift) {
    if (k4) {
      handle_jump_commands(d, dims);
    }
    if (colon) {
      d->mode = COMMAND;
    }
  }
}

