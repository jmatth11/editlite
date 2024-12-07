#include "key_handler.h"
#include "page.h"
#include "util.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>

void handle_row_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  const int c_row = d->cursor.pos.row;
  if (c_row >= cur_page->lines.len) {
    d->cursor.pos.row = cur_page->lines.len - 1;
  } else if (c_row >= cur_page->row_offset + dim.row) {
    cur_page->row_offset++;
  } else if (c_row < cur_page->row_offset) {
    cur_page->row_offset = c_row;
  }
  if ((c_row+10) >= cur_page->lines.len &&
    cur_page->file_offset_pos < cur_page->file_size) {
    d->page_mgr.read(cur_page);
  }
}

void handle_col_scroll(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  if (d->cursor.pos.col >= cur_line->chars.len) {
    d->cursor.pos.col = cur_line->chars.len - 1;
  } else if (d->cursor.pos.col >= cur_page->col_offset + dim.col) {
    cur_page->col_offset++;
  } else if (d->cursor.pos.col < cur_page->col_offset) {
    cur_page->col_offset = d->cursor.pos.col;
  }
}

void handle_jump_commands(struct display *d, struct display_dim dim) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  const int line_len = cur_line->chars.len - 1;
  const int offset = line_len - dim.col;
  d->cursor.pos.col = line_len;
  cur_page->col_offset = MAX(offset, 0);
}

void handle_simple_keypresses(struct display *d, struct win *w, SDL_Event *e) {
  struct display_dim dims;
  get_page_dim(d, w, &dims);
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
  }
}

void handle_state_keypresses(struct display *d, struct win *w, SDL_Event *e) {
  struct display_dim dims;
  get_page_dim(d, w, &dims);
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 lshift = key_states[SDL_SCANCODE_LSHIFT];
  const Uint8 rshift =key_states[SDL_SCANCODE_RSHIFT];
  const Uint8 k4 = key_states[SDL_SCANCODE_4];
  if ((lshift || rshift) && k4) {
    handle_jump_commands(d, dims);
  }
}

