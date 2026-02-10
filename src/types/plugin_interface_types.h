#ifndef EDITLITE_PLUGIN_INTERFACE_TYPES_H
#define EDITLITE_PLUGIN_INTERFACE_TYPES_H

#include "types/unicode_types.h"
#include <stddef.h>
struct page;

enum dispatch_calls {
  DISPATCH_EXIT = 0,
  DISPATCH_SAVE,
  DISPATCH_MENU,
  DISPATCH_NEW_PAGE,
  DISPATCH_PLUGIN_INSERT,
  DISPATCH_UPDATE_CURSOR,
  DISPATCH_ERROR_MESSAGE,
  DISPATCH_REGISTER_FUNCTION,
  DISPATCH_NORMAL,
};

struct plugin_interface {
  void *__internal;
  void (*get_cur_page)(struct plugin_interface*, struct page*);
  void (*dispatch)(struct plugin_interface*, const enum dispatch_calls, void*);
};

struct message_t {
  code_point_t *msg;
  size_t len;
};

#endif
