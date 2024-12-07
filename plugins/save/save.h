#ifndef EDITLITE_SAVE_PLUGIN_H
#define EDITLITE_SAVE_PLUGIN_H

#ifdef __cplusplus
extern "C"
#endif

#include <stdbool.h>
#include <SDL2/SDL_events.h>

struct plugin_interface;
struct display;
struct display_dim;

bool action(struct plugin_interface *d);
void get_display_prompt(const char **out);
bool render(struct display *, struct display_dim *);
bool event(SDL_Event *, struct display *, struct display_dim *);

#ifdef __cplusplus
}
#endif

#endif
