#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>

#include "components/display.h"
#include "modes/command_mode.h"
#include "modes/insert_mode.h"
#include "key_handler.h"
#include "structures/linked_list.h"
#include "scrolling.h"
#include "states/state.h"
#include "types/display_type.h"
#include "helpers/util.h"

void handle_plugin_textinput_mode(struct display*d, SDL_Event*e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
  }
  struct display_dim dims;
  state_get_page_dim(&d->state, &dims);
  // handle plugin events after our events
  if (d->state.cmds.len > 0) {
    for (int i = 0; i < d->state.cmds.len; ++i) {
      struct command *cmd = &d->state.cmds.command_data[i];
      if (cmd->event != NULL) {
        if (!cmd->event(e, d, &dims)) {
          const char *name;
          cmd->get_display_prompt(&name);
          fprintf(stderr, "command event failed: \"%s\"\n", name);
        }
      }
    }
  }
}

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
    case PLUGIN_INPUT: {
      handle_plugin_textinput_mode(d, e);
      break;
    }
    default:
    break;
  }
}

void handle_jump_commands(struct display *d, struct display_dim dim) {
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_jump_commands.\n");
    return;
  }
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines,
                                                     cur_page->cursor.pos.row);
  const int line_len = gap_buffer_get_len(&cur_line->value.chars) - 1;
  const int offset = line_len - dim.col;
  cur_page->cursor.pos.col = line_len;
  cur_page->page_offset.col = MAX(offset, 0);
}

void handle_simple_keypresses(struct display *d, SDL_Event *e) {
  struct display_dim dims;
  state_get_page_dim(&d->state, &dims);
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_jump_commands.\n");
    return;
  }
  switch (e->key.keysym.sym) {
    case SDLK_j:
    case SDLK_DOWN:
      cur_page->cursor.pos.row++;
      handle_row_scroll(d, dims);
      break;
    case SDLK_k:
    case SDLK_UP:
      cur_page->cursor.pos.row = MAX(0, cur_page->cursor.pos.row - 1);
      handle_row_scroll(d, dims);
      break;
    case SDLK_l:
    case SDLK_RIGHT:
      cur_page->cursor.pos.col++;
      handle_col_scroll(d, dims);
      break;
    case SDLK_h:
    case SDLK_LEFT: {
      if (cur_page->cursor.pos.col != cur_page->cursor.screen_pos.col) {
        cur_page->cursor.pos.col = cur_page->cursor.screen_pos.col;
      }
      cur_page->cursor.pos.col = MAX(0, cur_page->cursor.pos.col - 1);
      handle_col_scroll(d, dims);
      break;
    }
    case SDLK_0:
      cur_page->cursor.pos.col = 0;
      d->state.page_mgr.pages.page_data[d->state.cur_buf].page_offset.col = 0;
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
      cur_page->cursor.pos.row = 0;
      handle_row_scroll(d, dims);
      break;
    }
  }
}

void handle_state_keypresses(struct display *d, SDL_Event *e) {
  struct display_dim dims;
  state_get_page_dim(&d->state, &dims);
  struct page *cur_page;
  if (!state_get_cur_page(&d->state, &cur_page)) {
    fprintf(stderr, "could not get current page for handle_jump_commands.\n");
    return;
  }
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 shift = key_states[SDL_SCANCODE_LSHIFT] ||
    key_states[SDL_SCANCODE_RSHIFT];
  const Uint8 ctrl = key_states[SDL_SCANCODE_LCTRL] ||
    key_states[SDL_SCANCODE_RCTRL];
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
      cur_page->cursor.pos.row += (dims.row / 2);
      handle_row_scroll(d, dims);
    } else if (ku) {
      cur_page->cursor.pos.row -= (dims.row / 2);
      handle_row_scroll(d, dims);
    }
  }
}

