#ifndef EDITLITE_PLUGIN_EXIT_H
#define EDITLITE_PLUGIN_EXIT_H

#ifdef __cplusplus
extern "C"
#endif

#include <stdbool.h>
#include <plugin_interface.h>

bool action(struct plugin_interface *d);
void get_display_prompt(const char **out);

#endif
