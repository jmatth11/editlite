#include "win.h"
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>

void create_win(struct win *w) {
  w->window = SDL_CreateWindow("Editlite", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            w->width, w->height, SDL_WINDOW_SHOWN);
  if (w->window == NULL) {
    printf("Window is null\n");
  }
  w->screen = SDL_GetWindowSurface(w->window);
}

void free_win(struct win *w) {
  SDL_DestroyWindow(w->window);
  SDL_FreeSurface(w->screen);
}
