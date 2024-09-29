#ifndef EDITLITE_DISPLAY_H
#define EDITLITE_DISPLAY_H

#include <stdbool.h>
#include <SDL2/SDL_render.h>
#include <stdint.h>

#include "types/plugin_interface_types.h"
#include "types/unicode_types.h"

struct display;
struct display_dim;
struct page;
enum display_mode;

int display_init(struct display* d);
bool display_page_render(struct display *d);
void display_set_mode(struct display *d, enum display_mode mode);
SDL_Texture * handle_characters(struct display *d, const code_point_t cur_char);
struct viewable_page_info display_viewable_page_buffer(struct display *d, struct page *p);
struct plugin_interface display_plugin_interface(struct display *d);
void display_free(struct display *d);

#endif
