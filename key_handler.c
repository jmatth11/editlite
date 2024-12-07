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

void handle_keydown(struct display *d, SDL_Event *e) {
  switch(d->mode) {
    case NORMAL: {
      handle_state_keypresses(d, e);
      break;
    }
    case INSERT:{
      handle_insert_mode(d, e);
      break;
    }
    case COMMAND:{
      handle_command_mode(d, e);
      break;
    }
    default:
    break;
  }
}

void handle_jump_commands(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!display_get_cur_page(d, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_jump_commands.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  const int line_len = gap_buffer_get_len(&cur_line->value.chars) - 1;
  const int offset = line_len - dim.col;
  d->cursor.pos.col = line_len;
  cur_page->col_offset = MAX(offset, 0);
}

void handle_simple_keypresses(struct display *d, SDL_Event *e) {
  struct display_dim dims;
  display_get_page_dim(d, &dims);
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
      prepare_insert_mode(d, INSERT_AT);
      break;
    }
    case SDLK_a: {
      prepare_insert_mode(d, INSERT_AFTER);
      break;
    }
    case SDLK_g: {
      d->cursor.pos.row = 0;
      handle_row_scroll(d, dims);
      break;
    }
  }
}

void handle_state_keypresses(struct display *d, SDL_Event *e) {
  struct display_dim dims;
  display_get_page_dim(d, &dims);
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 shift = key_states[SDL_SCANCODE_LSHIFT] || key_states[SDL_SCANCODE_RSHIFT];
  const Uint8 ctrl = key_states[SDL_SCANCODE_LCTRL] || key_states[SDL_SCANCODE_RCTRL];
  const Uint8 k4 = key_states[SDL_SCANCODE_4];
  const Uint8 kg = key_states[SDL_SCANCODE_G];
  const Uint8 ki = key_states[SDL_SCANCODE_I];
  const Uint8 ka = key_states[SDL_SCANCODE_A];
  const Uint8 kd = key_states[SDL_SCANCODE_D];
  const Uint8 ku = key_states[SDL_SCANCODE_U];
  const Uint8 colon = key_states[SDL_SCANCODE_SEMICOLON];
  if (!shift || !ctrl) {
    handle_simple_keypresses(d, e);
  }
  if (shift) {
    if (k4) {
      handle_jump_commands(d, dims);
    }
    if (kg) {
      // TODO jump to end.
      // Will need to force a read of the file and then update row position.
    }
    if (ki) {
      prepare_insert_mode(d, INSERT_BEGIN);
    }
    if (ka) {
      prepare_insert_mode(d, INSERT_END);
    }
    if (colon) {
      d->mode = COMMAND;
      prepare_command_mode(d);
    }
  } else if (ctrl) {
    if (kd) {
      d->cursor.pos.row += (dims.row / 2);
      handle_row_scroll(d, dims);
    } else if (ku) {
      d->cursor.pos.row -= (dims.row / 2);
      handle_row_scroll(d, dims);
    }
  }
}

