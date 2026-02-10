#ifndef EDITLITE_STATE_TYPES_H
#define EDITLITE_STATE_TYPES_H

#include <stddef.h>
#include <stdbool.h>

#include "command_types.h"
#include "config_types.h"
#include "glyph_types.h"
#include "menu_types.h"
#include "page_types.h"
#include "plugin_interface_types.h"
#include "types/registered_functions.h"
#include "win_types.h"
#include "scribe.h"

struct app_state {
  struct config config;
  struct glyphs glyphs;
  struct menu menu;
  struct plugin_interface pi;
  struct win w;
  struct registry registry;
  struct Scribe_t scribe;

  struct command_array cmds;
  struct page_manager page_mgr;

  size_t cur_buf;
  bool running;
};

#endif
