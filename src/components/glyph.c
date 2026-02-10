#include <SDL2/SDL_render.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_ttf.h>

#include "glyph.h"
#include "structures/hash_map.h"
#include "types/glyph_types.h"
#include "types/unicode_types.h"
#include "types/win_types.h"
#include "utf8.h"

static SDL_Texture* create_glyph_texture(struct glyphs *ch, const struct win *w, const code_point_t point) {
  SDL_Surface *s = TTF_RenderGlyph32_Solid(ch->font, point, ch->color);
  if (s == NULL) {
    fprintf(stderr, "rendering TTF surface failed for code_point \"%d\". %s\n", point, SDL_GetError());
    return NULL;
  }
  SDL_Texture* result = SDL_CreateTextureFromSurface(w->renderer, s);
  SDL_FreeSurface(s);
  return result;
}

int init_char(struct glyphs *ch, const struct win *w, const char* ttf_file) {
  ch->font = TTF_OpenFont(ttf_file, ch->point);
  ch->unscaled_size.width = 0;
  ch->unscaled_size.height = 0;
  if (ch->font == NULL) {
    fprintf(stderr, "TTF font could not initialize.\n");
    exit(1);
  }
  ch->glyphs = hash_map_create(1000);
  if (ch->glyphs == NULL) {
    fprintf(stderr, "Glyph map could not initialize.\n");
    return 0;
  }
  for (int i = CHAR_START_RANGE; i < CHAR_END_RANGE; ++i) {
    SDL_Surface *s = TTF_RenderGlyph32_Blended(ch->font, i, ch->color);
    if (s == NULL) {
      fprintf(stderr, "rendering TTF surface failed. %s\n", SDL_GetError());
      continue;
    }
    if (s->h > ch->unscaled_size.height) {
      ch->unscaled_size.height = s->h;
    }
    if (s->w > ch->unscaled_size.width) {
      ch->unscaled_size.width = s->w;
    }
    SDL_Texture *new_glyph = SDL_CreateTextureFromSurface(w->renderer, s);
    const struct glyph_info new_char = {
      .point = i,
      .glyph = new_glyph,
    };
    if (new_glyph == NULL) {
      fprintf(stderr, "error with glyph: %s\n", SDL_GetError());
    } else {
      if (!hash_map_set(ch->glyphs, i, new_char)) {
        fprintf(stderr, "failed to insert new glyph: \"%c\".\n", i);
        return 0;
      }
    }
    SDL_FreeSurface(s);
  }
  SDL_Texture *new_glyph = create_glyph_texture(ch, w, replacement_character);
  const struct glyph_info new_char = {
    .point = replacement_character,
    .glyph = new_glyph,
  };
  if (new_glyph == NULL) {
    fprintf(stderr, "error with glyph: %s\n", SDL_GetError());
  } else {
    if (!hash_map_set(ch->glyphs, replacement_character, new_char)) {
      fprintf(stderr, "failed to insert new glyph: \"%c\".\n", replacement_character);
      return 0;
    }
  }
  if (ch->scale <= 0) {
    ch->scale = 1;
  }
  ch->scaled_size.width = (double)ch->unscaled_size.width * ch->scale;
  ch->scaled_size.height = (double)ch->unscaled_size.height * ch->scale;
  ch->sanitize_character = sanitize_character;
  ch->parse_sdl_input = code_point_from_sdl_input;
  return 0;
}

SDL_Texture* get_glyph(struct glyphs *ch, const struct win *w, const code_point_t c) {
  glyph_array points;
  points.len = 0;
  if (!hash_map_get(ch->glyphs, c, &points)) {
    const struct glyph_info new_glyph = {
      .point = c,
      .glyph = create_glyph_texture(ch, w, c),
    };
    if (new_glyph.glyph != NULL) {
      if (!hash_map_set(ch->glyphs, c, new_glyph)) {
        fprintf(stderr, "new glyph could not be inserted into hash map.\n");
      }
    }
    return new_glyph.glyph;
  }
  const size_t len = points.len;
  for (int i = 0; i < len; ++i) {
    const struct glyph_info *info = &points.glyph_data[i];
    if (info->point == c) {
      return info->glyph;
    }
  }
  if (c != replacement_character) {
    return get_glyph(ch, w, replacement_character);
  }
  return NULL;
}

void free_char(struct glyphs *ch) {
  hash_map_destroy(ch->glyphs);
  TTF_CloseFont(ch->font);
}

code_point_t code_point_from_sdl_input(SDL_Event *e) {
  code_point_t key = 0;
  const struct code_point point = utf8_next((uint8_t*)e->text.text, 32, 0);
  if (point.type != OCT_INVALID && (point.val != 0 && point.val != 1)) {
    key = point.val;
  } else {
    key = e->key.keysym.sym;
  }
  return sanitize_character(key);
}

code_point_t sanitize_character(code_point_t keycode) {
  code_point_t result = keycode;
  if (utf8_is_whitespace(keycode)) {
    if (keycode == '\r' || keycode == '\n') {
      result = '\n';
    } else {
      result = ' ';
    }
  }
  return result;
}

