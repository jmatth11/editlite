#ifndef EDITLITE_SAVE_PLUGIN_H
#define EDITLITE_SAVE_PLUGIN_H

#ifdef __cplusplus
extern "C"
#endif

#include <stdbool.h>

struct plugin_interface;

bool action(struct plugin_interface *d);
void get_display_prompt(const char **out);

#ifdef __cplusplus
}
#endif

#endif
