#include "find.h"
#include <page.h>
#include <deps/array_template/array_template.h>

const char *prompt = "find files";
struct find_info op;
bool showMenu = false;

bool setup(struct plugin_interface *pi) {
  return true;
}

bool action(struct plugin_interface *pi) {
  if (op.locs.location_data != NULL) {
    free_location_array(&op.locs);
  }
  init_location_array(&op.locs, 1);
  pi->dispatch(pi, DISPATCH_PLUGIN_INPUT, NULL);
  showMenu = true;
  return true;
}

bool render(struct display *d, struct display_dim *dim) {
  if (showMenu) {
    // TODO render find display
    // need to use popen to execute grep command and get output
  }
  return true;
}

bool event(SDL_Event *e, struct display *d) {
  if (e->type == SDL_QUIT) {
    showMenu = false;
  }

  return true;
}

void get_display_prompt(const char **out) {
  *out = prompt;
}

bool cleanup(struct plugin_interface* pi) {
  free_location_array(&op.locs);
  return true;
}
