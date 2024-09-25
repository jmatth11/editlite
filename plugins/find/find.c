#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_scancode.h>
#include <stdio.h>
#include <types/display_type.h>
#include <types/plugin_interface_types.h>

#include "draw.h"
#include "find.h"
#include "search.h"
#include "types/unicode_types.h"

const char *prompt = "find word";
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
  op.idx = 0;
  op.value_size = 0;
  op.visual_offset = 0;
  pi->dispatch(pi, DISPATCH_PLUGIN_INSERT, (void*)prompt);
  showMenu = true;
  return true;
}

bool render(struct display *d, struct display_dim *dim) {
  if (showMenu) {
    int winh, winw;
    SDL_GetWindowSize(d->state.w.window, &winw, &winh);
    const int menu_max_height = dim->row / 3.0f;
    const int line_height = d->state.glyphs.scaled_size.height;
    const int menu_len = op.locs.len < menu_max_height ? op.locs.len : menu_max_height;
    op.height = op.locs.len <= menu_max_height ?
      (op.locs.len * line_height) :
      (menu_max_height * line_height);
    // extra row for user input
    op.height += line_height;
    size_t options_y_offset = (winh - op.height) + line_height;
    draw_background(d, 0, winh - op.height, winw, op.height);
    draw_textinput(d, op.value, op.value_size, 0, winh - op.height, winw, line_height);
    draw_options(d, &op, menu_len, 0, options_y_offset, winw, line_height);
    size_t select_offset = (op.idx - op.visual_offset) < (menu_max_height-1) ? (op.idx - op.visual_offset) : (menu_max_height-1);
    size_t select_box_y_offset = (line_height * select_offset) + options_y_offset;
    draw_select_box(d, 0, select_box_y_offset, winw, line_height);
  }
  return true;
}

bool event(SDL_Event *e, struct display *d, struct display_dim *dim) {
  if (e->key.keysym.sym == SDLK_ESCAPE) {
    showMenu = false;
  }
  // only listen for keydown events
  if (e->type != SDL_KEYDOWN) return true;
  // - 1 for the text input row
  const int menu_max_height = (dim->row / 3.0f) - 1;
  const Uint8* key_states = SDL_GetKeyboardState(NULL);
  const Uint8 ctrl = key_states[SDL_SCANCODE_LCTRL] || key_states[SDL_SCANCODE_RCTRL];
  const Uint8 kn = key_states[SDL_SCANCODE_N];
  const Uint8 kp = key_states[SDL_SCANCODE_P];
  const Uint8 ky = key_states[SDL_SCANCODE_Y];
  if (ctrl) {
    if (kn) {
      ++op.idx;
      if (op.idx >= op.locs.len) op.idx = 0;
    }
    if (kp) {
      --op.idx;
      if (op.idx < 0) op.idx = op.locs.len - 1;
    }
    if (op.idx > (op.visual_offset + menu_max_height) && op.locs.len >= menu_max_height) {
      op.visual_offset = op.idx - menu_max_height;
    }
    if (op.idx < op.visual_offset) {
      if (op.idx == 0) {
        op.visual_offset = 0;
      } else {
        op.visual_offset -= (op.visual_offset - op.idx);
      }
    }
    if (ky) {
      struct display_dim new_pos;
      struct find_loc *loc = &op.locs.location_data[op.idx];
      new_pos.row = loc->line;
      new_pos.col = loc->beg;
      showMenu = false;
      d->state.pi.dispatch(&d->state.pi, DISPATCH_UPDATE_CURSOR, &new_pos);
      d->state.pi.dispatch(&d->state.pi, DISPATCH_NORMAL, NULL);
    }
  } else {
    if (e->key.keysym.sym == SDLK_BACKSPACE) {
      if (op.value_size > 0) {
        --op.value_size;
      }
    } else if (e->key.keysym.sym == SDLK_RETURN) {
      search_word_options(d, dim, &op);
      op.visual_offset = 0;
      op.idx = 0;
    } else {
      code_point_t input_c = d->state.glyphs.parse_sdl_input(e);
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
