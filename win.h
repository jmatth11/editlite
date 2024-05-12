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

int create_win(struct win *);
int clear(struct win *w, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void free_win(struct win *);

#endif
