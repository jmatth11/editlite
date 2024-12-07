#ifndef EDITLITE_WIN_H
#define EDITLITE_WIN_H

#include <SDL2/SDL_video.h>

struct win {
  SDL_Window *window;
  SDL_Surface *screen;
  int height;
  int width;
};

void create_win(struct win *);
void free_win(struct win *);

#endif
