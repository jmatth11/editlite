#include "insert_mode.h"
#include "gap_buffer.h"
#include "page.h"
#include <SDL2/SDL_keycode.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

static inline char sanitize_character(SDL_Keycode c) {
  // TODO look into SDL_TextInputEvent to see if that can replace this logic
  printf("keycode = \'%c\'", c);
  char result = '\0';
  if (c == SDLK_RETURN) return '\n';
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  bool is_upper = (key_states[SDL_SCANCODE_LSHIFT] || key_states[SDL_SCANCODE_RSHIFT]);
  if (isspace(c)) {
    if (c == '\r' || c == '\n') {
      // TODO handle newlines
    }
    result = ' ';
  } else if (isalpha(c)) {
    result = c;
    if (is_upper) {
      result = toupper(result);
    }
  } else if (isdigit(c)) {
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
    } else {
      switch (c) {
        case '-': {
          result = c;
          if (is_upper) {
            result = '_';
          }
          break;
        }
        case '=':{
          result = c;
          if (is_upper) {
            result = '+';
          }
          break;
        }
        case '[':{
          result = c;
          if (is_upper) {
            result = '{';
          }
          break;
        }
        case ']':{
          result = c;
          if (is_upper) {
            result = '}';
          }
          break;
        }
        case '\\':{
          result = c;
          if (is_upper) {
            result = '|';
          }
          break;
        }
        case ';':{
          result = c;
          if (is_upper) {
            result = ':';
          }
          break;
        }
        case '\'':{
          result = c;
          if (is_upper) {
            result = '"';
          }
          break;
        }
        case '`':{
          result = c;
          if (is_upper) {
            result = '~';
          }
          break;
        }
        case ',':{
          result = c;
          if (is_upper) {
            result = '<';
          }
          break;
        }
        case '.':{
          result = c;
          if (is_upper) {
            result = '>';
          }
          break;
        }
        case '/':{
          result = c;
          if (is_upper) {
            result = '?';
          }
          break;
        }
        case '*':
        case '+':
          result = c;
          break;
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
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  struct gap_buffer *cur_gb = &cur_line->chars;

  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    gap_buffer_delete(cur_gb);
    d->cursor.pos.col--;
  } else {
    const char received_char = sanitize_character(e->key.keysym.sym);
    printf("char = \'%c\'", received_char);
    if (received_char != '\0') {
      gap_buffer_insert(cur_gb, received_char);
      d->cursor.pos.col++;
    }
  }
}

void prepare_insert_mode(struct display *d, struct win *w) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  struct gap_buffer *cur_gb = &cur_line->chars;
  gap_buffer_move_cursor(cur_gb, d->cursor.pos.col);
}
