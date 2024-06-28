#include "plugin_interface.h"
#include "display.h"
#include "menu.h"
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
    case DISPATCH_SAVE: {
      struct page *p;
      display_get_cur_page(d, &p);
      if (!d->page_mgr.write(p)) {
        fprintf(stderr, "write failed for file \"%s\"\n", p->file_name);
      }
      d->mode = NORMAL;
      break;
    }
    case DISPATCH_MENU: {
      d->mode = COMMAND;
      if (d->menu.items.menu_item_data != NULL) {
        menu_free(&d->menu);
      }
      menu_init(&d->menu);

      menu_item_array *items = (menu_item_array*)context;
      // copy all values.
      for (int i = 0; i < items->len; ++i) {
        insert_menu_item_array(&d->menu.items, items->menu_item_data[i]);
      }
      break;
    }
    default:
      fprintf(stderr, "not implemented dispatch call.\n");
      break;
  }
}

void plugin_interface_init(struct plugin_interface* pi) {
  pi->get_cur_page = pi_get_cur_page;
  pi->dispatch = pi_dispatch;
}
