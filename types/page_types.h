#ifndef EDITLITE_PAGE_TYPES_H
#define EDITLITE_PAGE_TYPES_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL_events.h>
#include "../deps/array_template/array_template.h"
#include "cursor_types.h"
#include "size_types.h"
#include "types/unicode_types.h"

// forward declaration
struct linked_list;

struct page {
  FILE *fp;
  char *file_name;
  size_t file_size;
  size_t file_offset_pos;
  struct cursor cursor;
  struct display_dim page_offset;
  SDL_Point position_offset;
  bool (*handle_backspace)(struct page *, size_t row, size_t col);
  bool (*handle_keystroke)(struct page *, code_point_t, size_t row, size_t col);
  bool (*handle_newline)(struct page *, size_t row, size_t col);
  struct linked_list *lines;
};

generate_array_template(page, struct page)

struct page_manager {
  bool (*open)(struct page*);
  bool (*read)(struct page*, size_t limit);
  bool (*write)(struct page*);
  page_array pages;
};

struct viewable_page_info {
  const char *filename;
  struct utf8_buffer buffer;
};

#endif
