#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "page.h"
#include "win.h"

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("couldn't load video\n");
    exit(1);
  }
  if (TTF_Init() == -1) {
    printf("failed ttf init\n");
    exit(1);
  }
}

void deinit() {
  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char **arg) {
  init();
  struct win w = {
    .window = NULL,
    .renderer = NULL,
    .height = 480,
    .width = 640
  };
  create_win(&w);
  struct display d;
  if (init_display(&d, &w) != 0) {
    printf("Display could not initialize.\n");
    exit(1);
  }
  struct page_manager pm;
  if (init_page_manager(&pm) != 0) {
    printf("Page Manager could not initialize.\n");
    exit(1);
  }
  struct page test_buffer;
  if (init_page(&test_buffer) != 0) {
    printf("Page could not initialize.\n");
    exit(1);
  }
  insert_page_array(&pm.pages, test_buffer);
  if (pm.read_file(&pm.pages.page_data[0], "main.c") != 0) {
    printf("Could not read file.\n");
    exit(1);
  }
  d.page_mgr = pm;

  SDL_Event e; int quit = 0;
  while(!quit) {
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = 1;
      }
    }
    SDL_SetRenderDrawColor(w.renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(w.renderer);
    page_render(&d, &w);
    SDL_RenderPresent(w.renderer);
  }
  free_display(&d);
  free_win(&w);
  deinit();
  return 0;
}
