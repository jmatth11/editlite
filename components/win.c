#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

#include "types/win_types.h"

int win_init(struct win *w) {
  w->window = SDL_CreateWindow("Editlite", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            w->width, w->height, SDL_WINDOW_SHOWN);
  if (w->window == NULL) {
    fprintf(stderr, "Window is null\n");
    return -1;
  }
  w->renderer = SDL_CreateRenderer(w->window, -1, SDL_RENDERER_ACCELERATED);
  if (w->renderer == NULL) {
    fprintf(stderr, "renderer is null\n");
    return -1;
  }
  return 0;
}

void win_free(struct win *w) {
  SDL_DestroyRenderer(w->renderer);
  SDL_DestroyWindow(w->window);
}
