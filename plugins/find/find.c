#include "find.h"
#include "plugins/find/draw.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_scancode.h>
#include <page.h>
#include <deps/array_template/array_template.h>
#include <stdio.h>

const char *prompt = "find files";
struct find_info op;
bool showMenu = false;

bool setup(struct plugin_interface *pi) {
  return true;
}

bool action(struct plugin_interface *pi) {
  if (op.locs.location_data != NULL) {
    free_location_array(&op.locs);
  }
  init_location_array(&op.locs, 1);
  pi->dispatch(pi, DISPATCH_PLUGIN_INPUT, NULL);
  showMenu = true;
  return true;
}

bool render(struct display *d, struct display_dim *dim) {
  if (showMenu) {
    int winh, winw;
    SDL_GetWindowSize(d->w.window, &winw, &winh);
    const int menu_max_height = dim->row / 3.0f;
    const int line_height = d->glyphs.height;
    op.height = op.locs.len <= menu_max_height ?
      (op.locs.len * line_height) :
      (menu_max_height * line_height);
    // extra row for user input
    op.height += line_height;
    draw_background(d, 0, winh - op.height, winw, op.height);
    draw_textinput(d, op.value, op.value_size, 0, winh - op.height, winw, line_height);

    // TODO render find display
    // need to use popen to execute grep command and get output
  }
  return true;
}

bool event(SDL_Event *e, struct display *d) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    showMenu = false;
  }
  // only listen for keydown events
  if (e->type != SDL_KEYDOWN) return true;
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 ctrl = key_states[SDL_SCANCODE_LCTRL] || key_states[SDL_SCANCODE_RCTRL];
  const Uint8 kn = key_states[SDL_SCANCODE_N];
  const Uint8 kp = key_states[SDL_SCANCODE_P];
  if (ctrl) {
    // TODO check special commands
    if (kn) {

    }
    if (kp) {

    }
  } else {
    if (e->key.keysym.sym == SDLK_BACKSPACE) {
      if (op.value_size > 0) {
        --op.value_size;
      }
    } else {
      char input_c = d->glyphs.sanitize_character(e->key.keysym.sym);
      if (input_c != '\0' && input_c != '\n' && input_c != '\t') {
        if (op.value_size < FIND_INFO_VALUE_SIZE) {
          op.value[op.value_size] = input_c;
          ++op.value_size;
        }
      }
    }
  }
  return true;
}

void get_display_prompt(const char **out) {
  *out = prompt;
}

bool cleanup(struct plugin_interface* pi) {
  free_location_array(&op.locs);
  return true;
}
