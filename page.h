#ifndef EDITLITE_PAGE_H
#define EDITLITE_PAGE_H

#include <stdbool.h>

struct page;
struct page_manager;

bool page_init(struct page* p);
void page_reset_screen_cursor(struct page *p);
void page_free(struct page* p);

bool page_manager_init(struct page_manager* pm);
void page_manager_free(struct page_manager* pm);

#endif
