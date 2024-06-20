#ifndef EDITLITE_WIN_H
#define EDITLITE_WIN_H

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

struct win {
  SDL_Window *window;
  SDL_Renderer *renderer;
  int height;
  int width;
};

int win_init(struct win *);
void win_free(struct win *);

#endif
