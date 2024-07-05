#ifndef EDITLITE_PLUGIN_INTERFACE_H
#define EDITLITE_PLUGIN_INTERFACE_H

struct display;
struct plugin_interface;

void plugin_interface_init(struct plugin_interface*);
void plugin_interface_from_display(struct plugin_interface*, struct display*);

#endif
