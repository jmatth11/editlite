#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <types/display_type.h>
#include <types/page_types.h>
#include <types/plugin_interface_types.h>
#include <stddef.h>
#include <unistd.h>

#include "save.h"
#include "types/unicode_types.h"

#define FILENAME_LIMIT 4000
#define ERROR_LIMIT 200
#define TEST_FILE "editlite_test_perm_file"

struct save_info {
  int value_size;
  int err_size;
  code_point_t value[FILENAME_LIMIT];
  code_point_t err[ERROR_LIMIT];
};

struct message_t file_info = {
  .msg = NULL,
  .len = 0,
};

struct message_t error_info = {
  .msg = NULL,
  .len = 0,
};

const char *prompt = "save application";
bool showDialog = false;
struct save_info dialog;

static void draw_text(struct display *d, const char *text, const int len, const SDL_Rect dims) {
  SDL_Rect r = dims;
  for (int i = 0; i < len; ++i) {
    SDL_Texture *char_ren = d->texture_from_char(d, text[i]);
    if (char_ren == NULL) {
      char_ren = d->texture_from_char(d, '?');
    }
    SDL_RenderCopy(d->state.w.renderer, char_ren, NULL, &r);
    r.x += dims.w;
  }
}

static bool validate_filename(struct save_info *si) {
  // convert value to utf8
  if (access(si->value, F_OK) == 0) {
    memcpy(si->err, "File already exists.\0", 21);
    return false;
  }
  FILE *fp = fopen(TEST_FILE, "w+");
  if (fp == NULL) {
    memcpy(si->err, "File creation failed.\0", 22);
    return false;
  }
  fclose(fp);
  remove(TEST_FILE);
  return true;
}

bool action(struct plugin_interface *d) {
  struct page p;
  d->get_cur_page(d, &p);
  if (p.file_name == NULL) {
    d->dispatch(d, DISPATCH_PLUGIN_INSERT, NULL);
    showDialog = true;
  } else {
    d->dispatch(d, DISPATCH_SAVE, NULL);
    d->dispatch(d, DISPATCH_NORMAL, NULL);
  }
  return true;
}

void get_display_prompt(const char **out) {
  *out = prompt;
}

bool render(struct display *d, struct display_dim *dim) {
  if (!showDialog) return true;
  const size_t letter_height = d->state.glyphs.scaled_size.height;
  const size_t letter_width = d->state.glyphs.scaled_size.width;
  const size_t dialog_height = letter_height * 2;
  const size_t dialog_width = letter_width * dim->col;
  int winh, winw;
  SDL_GetWindowSize(d->state.w.window, &winw, &winh);
  SDL_Rect box = {
    .x = 0,
    .y = winh - dialog_height,
    .w = dialog_width,
    .h = dialog_height,
  };
  SDL_SetRenderDrawColor(d->state.w.renderer, 0x35, 0x35, 0x35, 0xFF);
  SDL_RenderFillRect(d->state.w.renderer, &box);
  draw_text(d, "Enter filename:", 15, (SDL_Rect){
    .x = 0,
    .y = winh-dialog_height,
    .w = letter_width,
    .h = letter_height,
  });
  draw_text(d, dialog.value, dialog.value_size, (SDL_Rect){
    .x = 0,
    .y = winh-(dialog_height-letter_height),
    .w = letter_width,
    .h = letter_height,
  });
  return true;
}

bool event(SDL_Event *e, struct display *d, struct display_dim *dim) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    showDialog = false;
  }
  if (e->type != SDL_KEYDOWN) return true;
  if (e->key.keysym.sym == SDLK_BACKSPACE) {
    if (dialog.value_size > 0) {
      --dialog.value_size;
    }
  } else if (e->key.keysym.sym == SDLK_RETURN) {
    // check to make sure a it's a valid filename
    if (validate_filename(&dialog)) {
      if (dialog.value_size < FILENAME_LIMIT) {
        dialog.value[dialog.value_size] = '\0';
      }
      file_info.msg = dialog.value;
      file_info.len = dialog.value_size;
      d->state.pi.dispatch(&d->state.pi, DISPATCH_SAVE, &file_info);
      d->state.pi.dispatch(&d->state.pi, DISPATCH_NORMAL, NULL);
      showDialog = false;
    } else {
      error_info.msg = dialog.err;
      error_info.len = dialog.value_size;
      d->state.pi.dispatch(&d->state.pi, DISPATCH_ERROR_MESSAGE, &error_info);
      d->state.pi.dispatch(&d->state.pi, DISPATCH_NORMAL, NULL);
    }
  } else {
    code_point_t input_c = d->state.glyphs.sanitize_character(e->key.keysym.sym);
    if (input_c != '\0' && input_c != '\n' && input_c != '\t') {
      if (dialog.value_size < FILENAME_LIMIT) {
        dialog.value[dialog.value_size] = input_c;
        ++dialog.value_size;
      }
    }
  }
  return true;
}
