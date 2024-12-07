#include "command_mode.h"

void handle_command_mode(struct display *d, struct win *w, SDL_Event *e) {
  switch (e->key.keysym.sym) {
    case SDLK_ESCAPE:
      d->mode = NORMAL;
      break;
  }
}

void prepare_command_mode(struct display *d, struct win *w) {

}
