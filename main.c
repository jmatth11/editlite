#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "command_prompt.h"
#include "config.h"
#include "key_handler.h"
#include "menu.h"
#include "page.h"
#include "util.h"
#include "win.h"
#include "display.h"

void init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "couldn't load video\n");
    exit(1);
  }
  if (TTF_Init() == -1) {
    fprintf(stderr, "failed ttf init\n");
    exit(1);
  }
}

void deinit() {
  TTF_Quit();
  SDL_Quit();
}

int main(int argc, char **argv) {
  init();
  char *file = NULL;
  if (argc > 1) {
    file = argv[1];
  }
  struct config config;
  config_init(&config);
  parse_config(&config);
  struct display d;
  d.config = config;
  d.w = (struct win){
    .window = NULL,
    .renderer = NULL,
    .height = config.win_height,
    .width = config.win_width
  };
  win_init(&d.w);
  if (display_init(&d) != 0) {
    fprintf(stderr, "Display could not initialize.\n");
    exit(1);
  }
  struct page_manager pm;
  if (page_manager_init(&pm) != 0) {
    fprintf(stderr, "Page Manager could not initialize.\n");
    exit(1);
  }
  struct page test_buffer;
  if (!page_init(&test_buffer)) {
    fprintf(stderr, "Page could not initialize.\n");
    exit(1);
  }
  if (file != NULL) {
    test_buffer.file_name = malloc(sizeof(char) * strlen(file));
    strcpy(test_buffer.file_name, file);
  }
  if (insert_page_array(&pm.pages, test_buffer) != 0) {
    fprintf(stderr, "error inserting page into page manager.\n");
    exit(1);
  }
  d.page_mgr = pm;

  clock_t init;
  SDL_Event e;
  while(d.running) {
    init = clock();
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        d.running = false;
      } else if (e.type == SDL_KEYDOWN) {
        handle_keydown(&d, &e);
      }
    }
    SDL_SetRenderDrawColor(d.w.renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(d.w.renderer);
    if (!display_page_render(&d)) {
      fprintf(stderr, "page render failed.\n");
    }
    if (d.mode == COMMAND) {
      menu_display(&d);
    }
    if (d.cmds.len > 0) {
      struct display_dim dim;
      display_get_page_dim(&d, &dim);
      for (int i = 0; i < d.cmds.len; ++i) {
        struct command *cmd = &d.cmds.command_data[i];
        if (cmd->render != NULL) {
          if (!cmd->render(&d, &dim)) {
            const char *name;
            cmd->get_display_prompt(&name);
            fprintf(stderr, "plugin render failed: \"%s\"\n", name);
          }
        }
      }
    }
    SDL_RenderPresent(d.w.renderer);
    double cur_exec_time = ((clock() - init) / (double)CLOCKS_PER_SEC);
    double tick = 1000.0 / d.config.fps;
    if (cur_exec_time < tick) {
      double delay = tick - cur_exec_time;
      SDL_Delay(delay);
    }
  }
  display_free(&d);
  deinit();
  return 0;
}
