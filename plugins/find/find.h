#ifndef EDITLITE_FIND_PLUGIN_H
#define EDITLITE_FIND_PLUGIN_H

#include <stdbool.h>
#include <plugin_interface.h>
#include <display.h>
#include <SDL2/SDL_events.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FIND_INFO_VALUE_SIZE 200
#define FIND_INFO_PREVIEW_SIZE 300

struct find_loc {
  int preview_size;
  char preview[FIND_INFO_PREVIEW_SIZE];
  size_t beg;
  size_t end;
};

generate_array_template(location, struct find_loc)

struct find_info {
  int idx;
  int height;
  int value_size;
  char value[FIND_INFO_VALUE_SIZE];
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
