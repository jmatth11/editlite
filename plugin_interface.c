#include "plugin_interface.h"
#include "display.h"
#include "page.h"

void pi_get_cur_page(struct plugin_interface* pi, struct page *p){
  struct display *d = (struct display*)pi->__internal;
  struct page *out;
  display_get_cur_page(d, &out);
  *p = *out;
}

void pi_dispatch(
  struct plugin_interface* pi,
  const enum dispatch_calls dc,
  void *context
) {
  struct display *d = (struct display*)pi->__internal;
  switch (dc) {
    case DISPATCH_EXIT:
      d->running = false;
      break;
    default:
      fprintf(stderr, "not implemented dispatch call.\n");
      break;
  }
}

void plugin_interface_init(struct plugin_interface* pi) {
  pi->get_cur_page = pi_get_cur_page;
  pi->dispatch = pi_dispatch;
}
