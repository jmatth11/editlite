#include "command_mode.h"
#include "command_prompt.h"
#include "display.h"
#include "plugin_interface.h"
#include <SDL2/SDL_keycode.h>

void execute_command(struct display *d) {
  struct command *cmd = &d->cmds.command_data[d->menu.idx];
  struct plugin_interface plugin;
  plugin_interface_init(&plugin);
  plugin.__internal = d;
  if (!cmd->action(&plugin)) {
    fprintf(stderr, "plugin failed action: \"%s\"\n", cmd->shared_library);
  }
}

void handle_command_mode(struct display *d, struct win *w, SDL_Event *e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
    case SDLK_RETURN:
      execute_command(d);
      break;
    case SDLK_j: {
        size_t cmd_len = d->cmds.len;
        ++d->menu.idx;
        if (d->menu.idx >= cmd_len) d->menu.idx = cmd_len - 1;
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

void prepare_command_mode(struct display *d, struct win *w) {
  // TODO figure out if I still need this.
}
