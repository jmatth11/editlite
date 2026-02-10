#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "components/display.h"
#include "components/menu.h"
#include "helpers/util.h"
#include "pages/page.h"
#include "plugin_interface.h"
#include "inputs/scrolling.h"
#include "states/registry.h"
#include "states/state.h"
#include "types/display_type.h"
#include "types/plugin_interface_types.h"
#include "types/page_types.h"
#include "types/registered_functions.h"
#include "types/state_types.h"
#include "utf8.h"

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
      const struct message_t* filename = (struct message_t*)context;
      if (filename != NULL && filename->msg != NULL) {
        p->file_name = (char*)encode_to_utf8(filename->msg, filename->len);
      }
      if (!d->state.page_mgr.write(p)) {
        fprintf(stderr, "write failed for file \"%s\"\n", p->file_name);
      }
      break;
    }
    case DISPATCH_MENU: {
      display_set_mode(d, COMMAND);
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
      const size_t filename_len = strlen(filename);
      bool found = false;
      for (int i = 0; i < d->state.page_mgr.pages.len; ++i) {
        struct page *tmp = &d->state.page_mgr.pages.page_data[i];
        if (tmp->file_name != NULL) {
          if (strncmp(tmp->file_name, filename, filename_len)==0) {
            d->state.cur_buf = i;
            found = true;
            break;
          }
        }
      }
      if (!found) {
        struct page p;
        if (!page_init(&p)) {
          fprintf(stderr, "error handling page: \"%s\"\n", filename);
          break;
        }
        p.file_name = (char*)malloc(sizeof(char)*filename_len+1);
        strncpy(p.file_name, filename, filename_len);
        p.file_name[filename_len] = '\0';
        insert_page_array(&d->state.page_mgr.pages, p);
        d->state.cur_buf = d->state.page_mgr.pages.len - 1;
      }
      display_set_mode(d, NORMAL);
      page_change_func_array events = d->state.registry.page_change_funcs;
      if (events.len > 0) {
        for (int i = 0; i < events.len; ++i) {
          page_change callback = events.page_change_func_data[i];
          if (callback != NULL) {
            callback(d, NEW_PAGE);
          }
        }
      }
      break;
    }
    case DISPATCH_PLUGIN_INSERT: {
      char *plugin_name = (char*)context;
      display_set_mode(d, PLUGIN_INSERT);
      d->plugin_mode_name = plugin_name;
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
      const struct message_t *err = (struct message_t*)context;
      uint8_t *msg = encode_to_utf8(err->msg, err->len);
      if (err != NULL) {
        fprintf(stderr, "error: %s\n", msg);
      }
      break;
    }
    case DISPATCH_REGISTER_FUNCTION: {
      const struct registry_entry* entry = (struct registry_entry*)context;
      if (!registry_add(&d->state.registry, entry)) {
        fprintf(stderr, "error adding plugins registry entry.\n");
      }
      break;
    }
    case DISPATCH_NORMAL: {
      display_set_mode(d, NORMAL);
      break;
    }
    default:
      fprintf(stderr, "not implemented dispatch call.\n");
      display_set_mode(d, NORMAL);
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
