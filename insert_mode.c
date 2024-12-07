#include "insert_mode.h"
#include "gap_buffer.h"
#include "page.h"
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
      // TODO handle newlines
    }
    result = ' ';
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
  struct line *cur_line = &cur_page->lines.line_data[d->cursor.pos.row];
  struct gap_buffer *cur_gb = &cur_line->chars;

  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    gap_buffer_delete(cur_gb);
    d->cursor.pos.col--;
  } else {
    const char received_char = sanitize_character(e->key.keysym.sym);
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
