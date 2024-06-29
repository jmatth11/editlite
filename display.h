#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include "glyph.h"
#include "page.h"
#include "config.h"
#include "command_prompt.h"
#include "plugin_interface.h"
#include <stdbool.h>
#include "menu.h"
#include "win.h"

enum display_mode {
  NORMAL = 0,
  INSERT,
  COMMAND,
};

struct display_dim {
  int col;
  int row;
};

struct cursor {
  struct display_dim pos;
  struct display_dim screen_pos;
};

struct display {
  enum display_mode mode;
  struct win w;
  bool running;
  size_t cur_buf;
  struct cursor cursor;
  struct glyphs glyphs;
  struct menu menu;
  struct command_array cmds;
  struct page_manager page_mgr;
  struct config config;
  struct plugin_interface pi;
};

int display_init(struct display* d);
bool display_load_plugins(struct display* d);
bool display_reload_plugins(struct display *d);
bool display_page_render(struct display *d, struct win *w);
bool display_get_cur_page(struct display *d, struct page **out);
void display_get_page_dim(struct display *d, struct win *w, struct display_dim *out);
SDL_Texture * handle_characters(struct display *d, const char cur_char);
struct plugin_interface display_plugin_interface(struct display *d);
void display_free(struct display *d);

#endif
