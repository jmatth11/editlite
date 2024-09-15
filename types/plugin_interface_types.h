#ifndef EDITLITE_PLUGIN_INTERFACE_TYPES_H
#define EDITLITE_PLUGIN_INTERFACE_TYPES_H

struct page;

enum dispatch_calls {
  DISPATCH_EXIT = 0,
  DISPATCH_SAVE,
  DISPATCH_MENU,
  DISPATCH_NEW_PAGE,
  DISPATCH_PLUGIN_INPUT,
  DISPATCH_UPDATE_CURSOR,
  DISPATCH_ERROR_MESSAGE,
  DISPATCH_REGISTER_FUNCTION,
};

struct plugin_interface {
  void *__internal;
  void (*get_cur_page)(struct plugin_interface*, struct page*);
  void (*dispatch)(struct plugin_interface*, const enum dispatch_calls, void*);
};

#endif
