#include "insert_mode.h"
#include "display.h"
#include "gap_buffer.h"
#include "linked_list.h"
#include "page.h"
#include "scrolling.h"
#include <SDL2/SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>

static inline char sanitize_character(SDL_Keycode keycode) {
  // TODO look into SDL_TextInputEvent to see if that can replace this logic
  char result = '\0';
  if (keycode == SDLK_LSHIFT || keycode == SDLK_RSHIFT) return result;
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  bool is_upper = (key_states[SDL_SCANCODE_LSHIFT] || key_states[SDL_SCANCODE_RSHIFT]);
  bool is_special_char = false;
  if (keycode > 127) {
    return result;
  }
  // force to char size
  char c = (char)keycode;
  switch (c) {
    case SDLK_MINUS: {
      result = c;
      if (is_upper) {
        result = '_';
      }
      is_special_char = true;
      break;
    }
    case SDLK_EQUALS:{
      result = c;
      if (is_upper) {
        result = '+';
      }
      is_special_char = true;
      break;
    }
    case SDLK_LEFTBRACKET:{
      result = c;
      if (is_upper) {
        result = '{';
      }
      is_special_char = true;
      break;
    }
    case SDLK_RIGHTBRACKET:{
      result = c;
      if (is_upper) {
        result = '}';
      }
      is_special_char = true;
      break;
    }
    case SDLK_BACKSLASH:{
      result = c;
      if (is_upper) {
        result = '|';
      }
      is_special_char = true;
      break;
    }
    case SDLK_SEMICOLON:{
      result = c;
      if (is_upper) {
        result = ':';
      }
      is_special_char = true;
      break;
    }
    case SDLK_QUOTE:{
      result = c;
      if (is_upper) {
        result = '"';
      }
      is_special_char = true;
      break;
    }
    case SDLK_BACKQUOTE:{
      result = c;
      if (is_upper) {
        result = '~';
      }
      is_special_char = true;
      break;
    }
    case SDLK_COMMA:{
      result = c;
      if (is_upper) {
        result = '<';
      }
      is_special_char = true;
      break;
    }
    case SDLK_PERIOD:{
      result = c;
      if (is_upper) {
        result = '>';
      }
      is_special_char = true;
      break;
    }
    case SDLK_SLASH:{
      result = c;
      if (is_upper) {
        result = '?';
      }
      is_special_char = true;
      break;
    }
    case SDLK_ASTERISK:
    case SDLK_PLUS:
      result = c;
      is_special_char = true;
      break;
  }
  if (!is_special_char && isspace(c)) {
    if (c == '\r' || c == '\n') {
      result = '\n';
    } else {
      result = ' ';
    }
  } else if (!is_special_char && isalpha(c)) {
    result = c;
    if (is_upper) {
      result = toupper(result);
    }
  } else if (!is_special_char && isdigit(c)) {
    result = c;
    if (is_upper) {
      switch (c) {
        case '1': result = '!'; break;
        case '2': result = '@'; break;
        case '3': result = '#'; break;
        case '4': result = '$'; break;
        case '5': result = '%'; break;
        case '6': result = '^'; break;
        case '7': result = '&'; break;
        case '8': result = '*'; break;
        case '9': result = '('; break;
        case '0': result = ')'; break;
      }
    }
  }
  return result;
}

void handle_insert_mode(struct display *d, struct win *w, SDL_Event *e) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    d->mode = NORMAL;
    return;
  }
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  struct display_dim dim;
  get_page_dim(d, w, &dim);

  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    if (d->cursor.pos.col > 0) {
      gap_buffer_delete(cur_gb);
      d->cursor.pos.col--;
    } else {
      struct linked_list *prev_line = cur_line->prev;
      struct gap_buffer *prev_gb = &prev_line->value.chars;
      gap_buffer_move_cursor(prev_gb, gap_buffer_get_len(prev_gb));
      // delete previous line's newline char
      gap_buffer_delete(prev_gb);
      size_t cur_gb_len = gap_buffer_get_len(cur_gb);
      for (int i = 0; i < cur_gb_len; ++i) {
        char tmp = ' ';
        gap_buffer_get_char(cur_gb, i, &tmp);
        gap_buffer_insert(prev_gb, tmp);
      }
      linked_list_delete_node(cur_line);
      d->cursor.pos.row--;
      d->cursor.pos.col = gap_buffer_get_len(prev_gb) - cur_gb_len;
      gap_buffer_move_cursor(prev_gb, d->cursor.pos.col);
    }
  } else {
    const char received_char = sanitize_character(e->key.keysym.sym);
    if (received_char != '\0') {
      if (received_char == '\n') {
        gap_buffer_insert(cur_gb, received_char);
        d->cursor.pos.col++;
        struct line new_line;
        init_line(&new_line);
        struct gap_buffer *next_gb = &new_line.chars;
        gap_buffer_move_cursor(next_gb, 0);
        size_t cur_col = d->cursor.pos.col;
        size_t gb_len = gap_buffer_get_len(cur_gb);
        for (; cur_col < gb_len; ++cur_col) {
          char tmp = ' ';
          gap_buffer_get_char(cur_gb, cur_col, &tmp);
          printf("%c", tmp);
          gap_buffer_insert(next_gb, tmp);
        }
        cur_col = d->cursor.pos.col;
        gap_buffer_move_cursor(cur_gb, gb_len - 1);
        gap_buffer_delete_seq(cur_gb, gb_len - cur_col);
        linked_list_insert(cur_line, 0, new_line);
        d->cursor.pos.col = 0;
        d->cursor.pos.row++;
        gap_buffer_move_cursor(next_gb, d->cursor.pos.col);
      } else {
        gap_buffer_insert(cur_gb, received_char);
        d->cursor.pos.col++;
      }
    }
    handle_row_scroll(d, dim);
    handle_col_scroll(d, dim);
  }

}

void prepare_insert_mode(struct display *d, struct win *w) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = linked_list_get_pos(cur_page->lines, d->cursor.pos.row);
  struct gap_buffer *cur_gb = &cur_line->value.chars;
  gap_buffer_move_cursor(cur_gb, d->cursor.pos.col);
}
