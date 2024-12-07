#include "win.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>

int create_win(struct win *w) {
  w->window = SDL_CreateWindow("Editlite", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            w->width, w->height, SDL_WINDOW_SHOWN);
  if (w->window == NULL) {
    printf("Window is null\n");
    return -1;
  }
  w->renderer = SDL_CreateRenderer(w->window, -1, SDL_RENDERER_ACCELERATED);
  if (w->renderer == NULL) {
    printf("renderer is null\n");
    return -1;
  }
  return 0;
}

int clear(struct win *w, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  return SDL_SetRenderDrawColor(w->renderer, r, g, b, a);
}

void free_win(struct win *w) {
  SDL_DestroyRenderer(w->renderer);
  SDL_DestroyWindow(w->window);
}
