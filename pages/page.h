#ifndef EDITLITE_PAGE_H
#define EDITLITE_PAGE_H

#include <stdbool.h>
#include <stdint.h>

struct page;
struct page_manager;
struct viewable_page_info;

bool page_init(struct page* p);
void page_reset_screen_cursor(struct page *p);
void page_free(struct page* p);

bool page_manager_init(struct page_manager* pm);
void page_manager_free(struct page_manager* pm);

void viewable_page_info_free(struct viewable_page_info*) __THROWNL __nonnull((1));

#endif
