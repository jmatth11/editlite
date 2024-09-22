#ifndef EDITLITE_FIND_PLUGIN_H
#define EDITLITE_FIND_PLUGIN_H

#include <deps/array_template/array_template.h>
#include <SDL2/SDL_events.h>
#include <stdbool.h>

struct display;
struct display_dim;
struct plugin_interface;

#ifdef __cplusplus
extern "C" {
#endif

#define FIND_INFO_VALUE_SIZE 200
#define FIND_INFO_PREVIEW_SIZE 300
#define FIND_INFO_PREVIEW_PADDING 5

struct find_loc {
  int preview_size;
  uint8_t preview[FIND_INFO_PREVIEW_SIZE];
  size_t line;
  size_t beg;
  size_t end;
};

generate_array_template(location, struct find_loc)

struct find_info {
  int idx;
  int visual_offset;
  int height;
  int value_size;
  uint8_t value[FIND_INFO_VALUE_SIZE];
  location_array locs;
};


bool setup(struct plugin_interface*);
bool action(struct plugin_interface *);
void get_display_prompt(const char **out);
bool render(struct display *, struct display_dim *);
bool event(SDL_Event *, struct display *, struct display_dim *);
bool cleanup(struct plugin_interface*);

#ifdef __cplusplus
}
#endif

#endif
