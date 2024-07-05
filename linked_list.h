#ifndef EDITLITE_LINKED_LIST_H
#define EDITLITE_LINKED_LIST_H

#include <stddef.h>
#include "line.h"

#ifdef __cplusplus
extern "C" {
#endif

struct linked_list {
  struct linked_list *next;
  struct linked_list *prev;
  struct line value;
};

int linked_list_init(struct linked_list *ll);
int linked_list_insert(struct linked_list *ll, size_t pos, struct line value);
int linked_list_append(struct linked_list *ll, struct line value);
struct linked_list* linked_list_get_pos(struct linked_list *ll, size_t pos);
struct linked_list* linked_list_get_end(struct linked_list *ll);
size_t linked_list_get_len(struct linked_list *ll);
int linked_list_delete(struct linked_list *ll, size_t pos);
int linked_list_delete_node(struct linked_list *ll);
void linked_list_free(struct linked_list *ll);
void linked_list_free_all(struct linked_list *ll);

#ifdef __cplusplus
}
#endif

#endif
