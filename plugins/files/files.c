#include "files.h"
#include <menu.h>

const char *prompt = "view files";
menu_item_array buffer;

bool setup(struct plugin_interface* pi) {
  return true;
}

bool action(struct plugin_interface *d) {
  //  TODO go through directory and display all files
  d->dispatch(d, DISPATCH_MENU, NULL);
  return true;
}
void get_display_prompt(const char **out) {
  *out = prompt;
}

bool cleanup(struct plugin_interface* pi) {
  if (buffer.menu_item_data != NULL) {
    free_menu_item_array(&buffer);
  }
  return true;
}
