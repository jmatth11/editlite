#ifndef EDITLITE_KEY_HANDLER_H
#define EDITLITE_KEY_HANDLER_H

#include <SDL2/SDL_events.h>

struct display;

void handle_keydown(struct display *d, SDL_Event *e);
void handle_simple_keypresses(struct display *d, SDL_Event *e);
void handle_state_keypresses(struct display *d, SDL_Event *e);

#endif
