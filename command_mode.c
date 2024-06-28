#include "command_mode.h"
#include "command_prompt.h"
#include "display.h"
#include "menu.h"
#include "plugin_interface.h"
#include <SDL2/SDL_keycode.h>

bool execute_command(struct display *d, void *ctx) {
  struct command *cmd = (struct command *)ctx;
  if (!cmd->action(&d->pi)) {
    fprintf(stderr, "plugin failed action: \"%s\"\n", cmd->shared_library);
    return false;
  }
  return true;
}

void handle_command_mode(struct display *d, struct win *w, SDL_Event *e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
    case SDLK_RETURN: {
      struct menu_item *mi = &d->menu.items.menu_item_data[d->menu.idx];
      mi->selected(d, mi->ctx);
      break;
    }
    case SDLK_j: {
        size_t len = d->menu.items.len;
        ++d->menu.idx;
        if (d->menu.idx >= len) d->menu.idx = len - 1;
        break;
    }
    case SDLK_k: {
        --d->menu.idx;
        if (d->menu.idx < 0) d->menu.idx = 0;
        break;
    }
    case SDLK_r:
      display_reload_plugins(d);
      break;
  }
}

void prepare_command_mode(struct display *d) {
  if (d->menu.items.menu_item_data != NULL) {
    menu_free(&d->menu);
  }
  if (!menu_init(&d->menu)) {
    fprintf(stderr, "menu failed to initialize.\n");
    return;
  }
  for (int i = 0; i < d->cmds.len; ++i) {
    struct command *cmd = &d->cmds.command_data[i];
    struct menu_item mi;
    cmd->get_display_prompt(&mi.name);
    mi.ctx = cmd;
    mi.selected = execute_command;
    insert_menu_item_array(&d->menu.items, mi);
  }
}

void leaving_command_mode(struct display *d) {
  menu_free(&d->menu);
}
