#include <SDL2/SDL_keycode.h>
#include "command_mode.h"
#include "commands/command.h"
#include "components/display.h"
#include "components/menu.h"
#include "states/state.h"
#include "types/display_type.h"

bool execute_command(struct display *d, void *ctx) {
  struct command *cmd = (struct command *)ctx;
  if (!cmd->action(&d->state.pi)) {
    fprintf(stderr, "plugin failed action: \"%s\"\n", cmd->shared_library);
    return false;
  }
  return true;
}

void handle_command_mode(struct display *d, SDL_Event *e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      display_set_mode(d, NORMAL);
      break;
    case SDLK_RETURN: {
      struct menu_item *mi = &d->state.menu.items.menu_item_data[d->state.menu.idx];
      mi->selected(d, mi->ctx);
      break;
    }
    case SDLK_j: {
        size_t len = d->state.menu.items.len;
        ++d->state.menu.idx;
        if (d->state.menu.idx >= len) d->state.menu.idx = len - 1;
        break;
    }
    case SDLK_k: {
        --d->state.menu.idx;
        if (d->state.menu.idx < 0) d->state.menu.idx = 0;
        break;
    }
    case SDLK_r:
      state_reload_plugins(&d->state);
      break;
  }
}

void prepare_command_mode(struct display *d) {
  if (d->state.menu.items.menu_item_data != NULL) {
    menu_free(&d->state.menu);
  }
  if (!menu_init(&d->state.menu)) {
    fprintf(stderr, "menu failed to initialize.\n");
    return;
  }
  for (int i = 0; i < d->state.cmds.len; ++i) {
    struct command *cmd = &d->state.cmds.command_data[i];
    struct menu_item mi;
    cmd->get_display_prompt(&mi.name);
    mi.ctx = cmd;
    mi.selected = execute_command;
    insert_menu_item_array(&d->state.menu.items, mi);
  }
}

void leaving_command_mode(struct display *d) {
  menu_free(&d->state.menu);
}
