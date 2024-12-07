#include "command_prompt.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

int display_command_prompt(struct command_prompt *cmd, struct display *d, struct win *w) {
  int win_h, win_w;
  SDL_GetWindowSize(w->window, &win_w, &win_h);
  SDL_Rect r = {
    .x = (win_w / 2) - (cmd->width / 2),
    .y = (win_h /2) - cmd->height,
    .h = cmd->height,
    .w = cmd->width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  return 0;
}
