#ifndef EDITLITE_FILES_PLUGIN_H
#define EDITLITE_FILES_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <plugin_interface.h>

bool setup(struct plugin_interface*);
bool action(struct plugin_interface *d);
void get_display_prompt(const char **out);
bool cleanup(struct plugin_interface*);

#ifdef __cplusplus
}
#endif

#endif
