#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_ttf.h>

#include "glyph.h"
#include "types/glyph_types.h"
#include "types/win_types.h"

int init_char(struct glyphs *ch, const struct win *w, const char* ttf_file) {
  ch->font = TTF_OpenFont(ttf_file, ch->point);
  ch->unscaled_size.width = 0;
  ch->unscaled_size.height = 0;
  if (ch->font == NULL) {
    fprintf(stderr, "TTF font could not initialize.\n");
    exit(1);
  }
  char glyph_buf[2];
  glyph_buf[1] = '\0';
  for (int i = CHAR_START_RANGE; i < CHAR_END_RANGE; ++i) {
    glyph_buf[0] = i;
    SDL_Surface *s = TTF_RenderText_Solid(ch->font, glyph_buf, ch->color);
    if (s == NULL) {
      fprintf(stderr, "rendering TTF surface failed. %s\n", SDL_GetError());
    }
    if (s->h > ch->unscaled_size.height) {
      ch->unscaled_size.height = s->h;
    }
    if (s->w > ch->unscaled_size.width) {
      ch->unscaled_size.width = s->w;
    }
    SDL_Texture *new_glyph = SDL_CreateTextureFromSurface(w->renderer, s);
    if (new_glyph == NULL) {
      ch->glyphs[i - CHAR_START_RANGE] = NULL;
      fprintf(stderr, "error with glyph: %s\n", SDL_GetError());
    } else {
      ch->glyphs[i - CHAR_START_RANGE] = new_glyph;
    }
    SDL_FreeSurface(s);
  }
  if (ch->scale <= 0) {
    ch->scale = 1;
  }
  ch->scaled_size.width = (double)ch->unscaled_size.width * ch->scale;
  ch->scaled_size.height = (double)ch->unscaled_size.height * ch->scale;
  ch->sanitize_character = sanitize_character;
  return 0;
}

SDL_Texture* get_glyph(struct glyphs *ch, const char c) {
  int idx = (int)c - CHAR_START_RANGE;
  // TODO allow out of range character generation for unicode
  if (idx >= 0 && idx < CHAR_END_RANGE - CHAR_START_RANGE) {
    return ch->glyphs[idx];
  }
  return NULL;
}

void free_char(struct glyphs *ch) {
  for (int i = 0; i < (CHAR_END_RANGE - CHAR_START_RANGE); ++i) {
    SDL_Texture *tmp = ch->glyphs[i];
    if (tmp != NULL) {
      SDL_DestroyTexture(tmp);
    }
  }
  TTF_CloseFont(ch->font);
}

char sanitize_character(SDL_Keycode keycode) {
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
  // TODO switch to using <unicode/uchar.h>
  // u_isUAlphabetic, u_isalpha etc.
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

