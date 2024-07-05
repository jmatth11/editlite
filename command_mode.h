#ifndef EDITLITE_COMMAN_MODE_H
#define EDITLITE_COMMAN_MODE_H

#include <SDL2/SDL_events.h>

struct display;

void handle_command_mode(struct display *d, SDL_Event *e);
void prepare_command_mode(struct display *d);
void leaving_command_mode(struct display *d);

#endif
