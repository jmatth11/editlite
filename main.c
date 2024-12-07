#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "win.h"

int main(int argc, char **arg) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("couldn't load video\n");
    return 1;
  }
  struct win w = {
    .window = NULL,
    .screen = NULL,
    .height = 480,
    .width = 640
  };
  create_win(&w);
  SDL_FillRect(w.screen, NULL, SDL_MapRGB(w.screen->format, 0, 0, 0));
  SDL_UpdateWindowSurface(w.window);
  SDL_Event e; int quit = 0;
  while(!quit) {
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = 1;
      }
    }
  }
  free_win(&w);
  return 0;
}
