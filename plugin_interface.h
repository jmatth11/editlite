#ifndef EDITLITE_PLUGIN_INTERFACE_H
#define EDITLITE_PLUGIN_INTERFACE_H

#include "page.h"

enum dispatch_calls {
  DISPATCH_EXIT = 0,
  DISPATCH_SAVE,
  DISPATCH_MENU,
  DISPATCH_NEW_PAGE,
  DISPATCH_PLUGIN_INPUT,
  DISPATCH_UPDATE_CURSOR,
};

struct plugin_interface {
  void *__internal;
  void (*get_cur_page)(struct plugin_interface*, struct page*);
  void (*dispatch)(struct plugin_interface*, const enum dispatch_calls, void*);
};

void plugin_interface_init(struct plugin_interface*);
void plugin_interface_from_display(struct plugin_interface*, struct display*);

#endif
