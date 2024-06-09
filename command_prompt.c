#include "command_prompt.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

int display_command_prompt(struct command_prompt *cmd, struct display *d, struct win *w) {
  SDL_Rect r = {
    .x = (w->width / 2) - (cmd->width / 2),
    .y = (w->height /2) - cmd->height,
    .h = cmd->height,
    .w = cmd->width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  return 0;
}
