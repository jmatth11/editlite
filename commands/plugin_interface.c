#include <string.h>

#include "components/display.h"
#include "components/menu.h"
#include "pages/page.h"
#include "plugin_interface.h"
#include "inputs/scrolling.h"
#include "states/state.h"
#include "types/display_type.h"
#include "types/plugin_interface_types.h"
#include "types/page_types.h"
#include "types/state_types.h"

void pi_get_cur_page(struct plugin_interface* pi, struct page *p){
  struct display *d = (struct display*)pi->__internal;
  struct page *out;
  state_get_cur_page(&d->state, &out);
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
      d->state.running = false;
      break;
    case DISPATCH_SAVE: {
      struct page *p;
      state_get_cur_page(&d->state, &p);
      const char* filename = (char*)context;
      if (filename != NULL) {
        const size_t len = strlen(filename);
        p->file_name = malloc(sizeof(char)*len);
        strcpy(p->file_name, filename);
      }
      if (!d->state.page_mgr.write(p)) {
        fprintf(stderr, "write failed for file \"%s\"\n", p->file_name);
      }
      d->mode = NORMAL;
      break;
    }
    case DISPATCH_MENU: {
      d->mode = COMMAND;
      if (d->state.menu.items.menu_item_data != NULL) {
        menu_free(&d->state.menu);
      }
      menu_init(&d->state.menu);

      menu_item_array *items = (menu_item_array*)context;
      // copy all values.
      for (int i = 0; i < items->len; ++i) {
        insert_menu_item_array(&d->state.menu.items, items->menu_item_data[i]);
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
      for (int i = 0; i < d->state.page_mgr.pages.len; ++i) {
        struct page *tmp = &d->state.page_mgr.pages.page_data[i];
        if (tmp->file_name != NULL) {
          if (strcmp(tmp->file_name, filename)==0) {
            d->state.cur_buf = i;
            found = true;
            break;
          }
        }
      }
      if (!found) {
        p.file_name = malloc(sizeof(char)*strlen(filename));
        strcpy(p.file_name, filename);
        insert_page_array(&d->state.page_mgr.pages, p);
        d->state.cur_buf = d->state.page_mgr.pages.len - 1;
      }
      d->mode = NORMAL;
      break;
    }
    case DISPATCH_PLUGIN_INPUT: {
      d->mode = PLUGIN_INPUT;
      break;
    }
    case DISPATCH_UPDATE_CURSOR: {
      struct display_dim *dim = (struct display_dim*)context;
      struct page *cur_page;
      if (!state_get_cur_page(&d->state, &cur_page)) {
        fprintf(stderr, "could not get current page for handle_insert_mode.\n");
        return;
      }
      cur_page->cursor.pos = *dim;
      struct display_dim win_dim;
      state_get_page_dim(&d->state, &win_dim);
      // row needs to be first in case we need to load in more of the file
      handle_row_scroll(d, win_dim);
      handle_col_scroll(d, win_dim);
      break;
    }
    case DISPATCH_ERROR_MESSAGE: {
      // TODO add proper support for error messages
      const char *err = (char*)context;
      if (err != NULL) {
        fprintf(stderr, "error: %s\n", err);
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

void plugin_interface_from_display(struct plugin_interface* pi, struct display* d) {
  plugin_interface_init(pi);
  pi->__internal = d;
}
