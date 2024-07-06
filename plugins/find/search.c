#include <structures/linked_list.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <types/display_type.h>
#include <types/page_types.h>

#include "find.h"
#include "search.h"

char cur_buffer[BUFSIZ];

bool check_and_add(struct display_dim *dim, const char *src, size_t src_len, const char *val, size_t val_size, struct find_loc *out) {
  if (src_len < val_size) return false;
  size_t found_idx = 0;
  bool result = false;
  for (int i = 0; i<src_len; ++i) {
    if (found_idx == val_size) {
      size_t val_start = i - val_size;
      size_t val_end = found_idx;
      size_t max_len = src_len;
      if (max_len > dim->col) {
        max_len = dim->col;
        if (max_len > FIND_INFO_PREVIEW_SIZE) {
          max_len = FIND_INFO_PREVIEW_SIZE;
        }
      }
      out->beg = val_start;
      out->end = val_end;
      out->preview_size = max_len;
      if (max_len < dim->col) {
        strncpy(out->preview, src, max_len);
      } else if (src_len > dim->col) {
        // if the value is bigger than our buffer, just show as much of the value as possible.
        if ((val_size + FIND_INFO_PREVIEW_PADDING) > max_len) {
          strncpy(out->preview, val, max_len);
        } else {
          size_t src_start = (max_len - FIND_INFO_PREVIEW_PADDING) - val_start;
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
  struct page *cur_page = &d->state.page_mgr.pages.page_data[d->state.cur_buf];
  struct linked_list *cur_line = cur_page->lines;
  size_t line_idx = 0;
  while (cur_line != NULL) {
    struct gap_buffer *gb = &cur_line->value.chars;
    char *buf = gb->get_str(gb);
    size_t buf_len = strlen(buf);
    struct find_loc loc;
    if (check_and_add(dim, buf, buf_len, op->value, op->value_size, &loc)) {
      loc.line = line_idx;
      insert_location_array(&op->locs, loc);
    }
    free(buf);
    cur_line = cur_line->next;
    ++line_idx;
  }
  // break if file is fully in memory
  if (cur_page->file_offset_pos >= cur_page->file_size) return;
  // handle reading file that is still on disk
  FILE *fp = fopen(cur_page->file_name, "r");
  if (fp == NULL) {
    fprintf(stderr, "could not open file for find plugin: \"%s\"\n", cur_page->file_name);
    return;
  }
  fseek(fp, cur_page->file_offset_pos, SEEK_SET);
  size_t n = 0;
  size_t total = 0;
  bool done = false;
  ++line_idx;
  while (!done) {
    n = fread(cur_buffer, sizeof(char), BUFSIZ, fp);
    if (n < BUFSIZ) {
      done = true;
    }
    size_t start_idx = 0;
    size_t line_len = 0;
    for (int i = 0; i < n; ++i) {
      struct find_loc loc;
      ++line_len;
      if (cur_buffer[i] == '\n') {
        if (check_and_add(dim, &cur_buffer[start_idx], line_len, op->value, op->value_size, &loc)) {
          loc.line = line_idx;
          insert_location_array(&op->locs, loc);
        }
        start_idx = i + 1;
        ++line_idx;
        line_len = 0;
      }
    }
    // if we didn't hit a newline at the end of our buffer,
    // rewind to the last known newline, except in the case of no newline.
    if (start_idx < n && (n - start_idx) != n) {
      total += start_idx;
      fseek(fp, cur_page->file_offset_pos + total, SEEK_SET);
    } else {
      total += n;
    }
  }
}
