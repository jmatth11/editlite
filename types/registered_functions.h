#ifndef EDITLITE_REGISTERED_FUNCTIONS_H
#define EDITLITE_REGISTERED_FUNCTIONS_H

#include "deps/array_template/array_template.h"
#include <SDL2/SDL_events.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declaration
struct display;
struct character_display;

enum page_change_type {
  NEW_PAGE,
  VIEW_RANGE_MOVE,
};

// function typedefs
typedef void (*insert_event)(struct display *, const SDL_Event *e);
typedef void (*render_glyph)(struct character_display *);
typedef void (*page_change)(struct display *, enum page_change_type);

generate_array_template(insert_func, insert_event)
generate_array_template(render_glyph_func, render_glyph)
generate_array_template(page_change_func, page_change)

/**
 * The types of functions that can be registered.
 */
enum registry_types {
  /* For insert events on the current page. */
  REG_INSERT,
  REG_RENDER_GLYPH,
  REG_PAGE_CHANGE,
};

/**
 * Structure for plugins to register a callback.
 */
struct registry_entry {
  /* The callback function. */
  void *callback;
  /* The type it is to be stored properly. */
  enum registry_types type;
};

struct registry {
  insert_func_array insert_funcs;
  render_glyph_func_array render_glyph_funcs;
  page_change_func_array page_change_funcs;
};

__END_DECLS

#endif
