#include "search.h"
#include "page.h"
#include <string.h>


void search_word_options(struct display *d, struct find_info *op) {
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = cur_page->lines;
  while (cur_line != NULL) {
    struct gap_buffer *gb = &cur_line->value.chars;
    char *buf = gb->get_str(gb);
    size_t buf_len = strlen(buf);
    for (int i = 0; i<buf_len; ++i) {
      // TODO finish string compare and search
    }
    free(buf);
  }
}
