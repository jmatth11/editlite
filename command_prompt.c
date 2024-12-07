#include "command_prompt.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdlib.h>

bool command_prompt_display(struct display *d, struct win *w) {
  int win_h, win_w;
  SDL_GetWindowSize(w->window, &win_w, &win_h);
  const int width = win_w - 50;
  const int height = 60;
  const int x_offset = (win_w / 2) - (width / 2);
  const int y_offset = (win_h /2) - height;
  SDL_Rect r = {
    .x = x_offset,
    .y = y_offset,
    .h = height,
    .w = width,
  };
  SDL_SetRenderDrawColor(w->renderer, 0x35, 0x35, 0x35, 0xff);
  SDL_RenderFillRect(w->renderer, &r);
  return 0;
}
