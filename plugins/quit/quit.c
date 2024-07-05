#include <types/plugin_interface_types.h>
#include <stddef.h>

#include "quit.h"

const char *prompt = "quit application";

bool action(struct plugin_interface *d) {
  d->dispatch(d, DISPATCH_EXIT, NULL);
  return true;
}
void get_display_prompt(const char **out) {
  *out = prompt;
}
