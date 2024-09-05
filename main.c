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

#include "components/display.h"
#include "components/menu.h"
#include "components/win.h"
#include "helpers/config.h"
#include "inputs/key_handler.h"
#include "pages/page.h"
#include "types/config_types.h"
#include "types/display_type.h"
#include "types/win_types.h"
#include "states/state.h"

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
  d.state.config = config;
  d.state.w = (struct win){
    .window = NULL,
    .renderer = NULL,
    .height = config.win_height,
    .width = config.win_width
  };
  win_init(&d.state.w);
  if (display_init(&d) != 0) {
    fprintf(stderr, "Display could not initialize.\n");
    exit(1);
  }
  // page_mgr expects at least one page in it
  struct page test_buffer;
  if (!page_init(&test_buffer)) {
    fprintf(stderr, "Page could not initialize.\n");
    exit(1);
  }
  // if the user supplies a file, load it in
  if (file != NULL) {
    size_t file_len = strlen(file);
    test_buffer.file_name = malloc(sizeof(char) * file_len + 1);
    strncpy(test_buffer.file_name, file, file_len);
    test_buffer.file_name[file_len] = '\0';
  }
  // insert our first page
  if (!insert_page_array(&d.state.page_mgr.pages, test_buffer)) {
    fprintf(stderr, "error inserting page into page manager.\n");
    exit(1);
  }

  clock_t init;
  SDL_Event e;
  while(d.state.running) {
    init = clock();
    while(SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        d.state.running = false;
      } else if (e.type == SDL_KEYDOWN) {
        handle_keydown(&d, &e);
      }
    }
    SDL_SetRenderDrawColor(d.state.w.renderer, 0, 0, 0, 0xFF);
    SDL_RenderClear(d.state.w.renderer);
    if (!display_page_render(&d)) {
      fprintf(stderr, "page render failed.\n");
    }
    if (d.mode == COMMAND) {
      menu_display(&d);
    }
    if (d.state.cmds.len > 0) {
      struct display_dim dim;
      state_get_page_dim(&d.state, &dim);
      for (int i = 0; i < d.state.cmds.len; ++i) {
        struct command *cmd = &d.state.cmds.command_data[i];
        if (cmd->render != NULL) {
          if (!cmd->render(&d, &dim)) {
            const char *name;
            cmd->get_display_prompt(&name);
            fprintf(stderr, "plugin render failed: \"%s\"\n", name);
          }
        }
      }
    }
    SDL_RenderPresent(d.state.w.renderer);
    double cur_exec_time = ((clock() - init) / (double)CLOCKS_PER_SEC);
    double tick = 1000.0 / d.state.config.fps;
    if (cur_exec_time < tick) {
      double delay = tick - cur_exec_time;
      SDL_Delay(delay);
    }
  }
  display_free(&d);
  deinit();
  return 0;
}
