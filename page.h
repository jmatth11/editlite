#ifndef EDITLITE_PAGE_H
#define EDITLITE_PAGE_H

#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <stddef.h>
#include "deps/array_template/array_template.h"
#include "linked_list.h"

// forward declaration
struct display;

struct page {
  FILE *fp;
  char *file_name;
  size_t file_offset_pos;
  size_t file_size;
  int col_offset;
  int row_offset;
  int x_offset;
  int y_offset;
  bool (*handle_backspace)(struct page *, struct display *d);
  bool (*handle_keystroke)(struct page *, struct display *d, SDL_Event *e);
  struct linked_list *lines;
};

bool page_init(struct page* p);
void page_free(struct page* p);

generate_array_template(page, struct page)

struct page_manager {
  page_array pages;
  bool (*open)(struct page*);
  bool (*read)(struct page*, size_t limit);
  bool (*write)(struct page*);
};

int page_manager_init(struct page_manager* pm);
void page_manager_free(struct page_manager* pm);

#endif
