#include "plugin_interface.h"
#include "display.h"
#include "menu.h"
#include "page.h"
#include <string.h>

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
    case DISPATCH_NEW_PAGE: {
      char *filename = (char*)context;
      struct page p;
      if (!page_init(&p)) {
        fprintf(stderr, "error handling page: \"%s\"\n", filename);
        break;
      }
      bool found = false;
      for (int i = 0; i < d->page_mgr.pages.len; ++i) {
        struct page *tmp = &d->page_mgr.pages.page_data[i];
        if (tmp->file_name != NULL) {
          if (strcmp(tmp->file_name, filename)==0) {
            d->cur_buf = i;
            found = true;
            break;
          }
        }
      }
      if (!found) {
        p.file_name = malloc(sizeof(char)*strlen(filename));
        strcpy(p.file_name, filename);
        insert_page_array(&d->page_mgr.pages, p);
        d->cur_buf = d->page_mgr.pages.len - 1;
        // TODO think about moving the cursor to the page level so
        // we can remember where we left off.
        d->cursor.pos.col = 0;
        d->cursor.pos.row = 0;
      }
      d->mode = NORMAL;
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

void plugin_interface_from_display(struct plugin_interface* pi, struct display* d) {
  plugin_interface_init(pi);
  pi->__internal = d;
}
