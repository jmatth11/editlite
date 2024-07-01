#ifndef EDITLITE_FIND_PLUGIN_H
#define EDITLITE_FIND_PLUGIN_H

#include <stdbool.h>
#include <plugin_interface.h>
#include <display.h>
#include <SDL2/SDL_events.h>

#ifdef __cplusplus
extern "C" {
#endif

struct find_loc {
  size_t beg;
  size_t end;
};

generate_array_template(location, struct find_loc)

struct find_info {
  char *value;
  location_array locs;
};


bool setup(struct plugin_interface*);
bool action(struct plugin_interface *);
void get_display_prompt(const char **out);
bool render(struct display *, struct display_dim *);
bool event(SDL_Event *, struct display *);
bool cleanup(struct plugin_interface*);

#ifdef __cplusplus
}
#endif

#endif
