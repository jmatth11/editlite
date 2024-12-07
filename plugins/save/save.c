#include "save.h"

const char *prompt = "save application";

bool action(struct plugin_interface *d) {
  d->dispatch(d, DISPATCH_SAVE, NULL);
  return true;
}
void get_display_prompt(const char **out) {
  *out = prompt;
}
