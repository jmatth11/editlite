#include "search.h"
#include "page.h"
#include "find.h"
#include <stdbool.h>
#include <string.h>

bool check_and_add(struct display_dim *dim, const char *src, size_t src_len, const char *val, size_t val_size, struct find_loc *out) {
  if (src_len < val_size) return false;
  size_t found_idx = 0;
  bool result = false;
  for (int i = 0; i<src_len; ++i) {
    if (found_idx == val_size) {
      size_t val_start = i - val_size;
      size_t val_end = found_idx;
      size_t max_len = dim->col < FIND_INFO_PREVIEW_SIZE ? dim->col : FIND_INFO_PREVIEW_SIZE;
      out->beg = val_start;
      out->end = val_end;
      out->preview_size = max_len;
      if (src_len < dim->col) {
        strncpy(out->preview, src, max_len);
      } else if (src_len > dim->col) {
        if ((val_size + FIND_INFO_PREVIEW_PADDING) > max_len) {
          strncpy(out->preview, val, max_len);
        } else {
          size_t src_start = val_start - (max_len - FIND_INFO_PREVIEW_PADDING);
          strncpy(out->preview, &src[src_start], max_len);
        }
      }
      result = true;
      break;
    }
    if (src[i] == val[found_idx]) {
      ++found_idx;
    } else {
      found_idx = 0;
    }
  }
  return result;
}

void search_word_options(struct display *d, struct display_dim *dim, struct find_info *op) {
  if (op->locs.location_data != NULL) {
    free_location_array(&op->locs);
  }
  init_location_array(&op->locs, 1);
  struct page *cur_page = &d->page_mgr.pages.page_data[d->cur_buf];
  struct linked_list *cur_line = cur_page->lines;
  while (cur_line != NULL) {
    struct gap_buffer *gb = &cur_line->value.chars;
    char *buf = gb->get_str(gb);
    size_t buf_len = strlen(buf);
    struct find_loc loc;
    if (check_and_add(dim, buf, buf_len, op->value, op->value_size, &loc)) {
      insert_location_array(&op->locs, loc);
    }
    free(buf);
    cur_line = cur_line->next;
  }
}
