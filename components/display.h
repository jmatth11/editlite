#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL_render.h>

#include "types/plugin_interface_types.h"

struct display;
struct display_dim;
struct page;

int display_init(struct display* d);
bool display_page_render(struct display *d);
SDL_Texture * handle_characters(struct display *d, const char cur_char);
struct plugin_interface display_plugin_interface(struct display *d);
void display_free(struct display *d);

#endif
