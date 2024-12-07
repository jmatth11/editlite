#include <stdlib.h>
#include "line.h"
#include "linked_list.h"

int linked_list_init(struct linked_list *ll) {
  init_line(&ll->value);
  ll->next = NULL;
  ll->prev = NULL;
  return 0;
}

static struct linked_list* find_pos(struct linked_list *ll, size_t target_pos) {
  if (ll == NULL) return NULL;
  struct linked_list *cur = ll;
  size_t idx = 0;
  while (cur != NULL) {
    if (idx == target_pos) {
      break;
    }
    cur = cur->next;
    ++idx;
  }
  if (idx != target_pos) {
    return NULL;
  }
  return cur;
}

static struct linked_list* find_end(struct linked_list *ll) {
  if (ll == NULL) return NULL;
  struct linked_list *cur = ll;
  while (cur->next != NULL) {
    cur = cur->next;
  }
  return cur;
}

static inline size_t find_len(struct linked_list *ll) {
  struct linked_list *cur = ll;
  size_t result = 0;
  while(cur != NULL) {
    ++result;
    cur = cur->next;
  }
  return result;
}

int linked_list_insert(struct linked_list *ll, size_t pos, struct line value) {
  struct linked_list *obj = find_pos(ll, pos);
  if (obj == NULL) return 1;
  struct linked_list *tmp = (struct linked_list*)malloc(sizeof(struct linked_list));
  if (tmp == NULL) return 1;
  linked_list_init(tmp);
  tmp->prev = obj;
  tmp->value = value;
  if (obj->next != NULL) {
    obj->next->prev = tmp;
    tmp->next = obj->next;
  }
  obj->next = tmp;
  return 0;
}

int linked_list_append(struct linked_list *ll, struct line value) {
  struct linked_list *obj = find_end(ll);
  if (obj == NULL) return 1;
  struct linked_list *tmp = (struct linked_list*)malloc(sizeof(struct linked_list));
  if (tmp == NULL) return 1;
  linked_list_init(tmp);
  tmp->prev = obj;
  tmp->value = value;
  obj->next = tmp;
  return 0;
}

struct linked_list* linked_list_get_pos(struct linked_list *ll, size_t pos) {
  return find_pos(ll, pos);
}

struct linked_list* linked_list_get_end(struct linked_list *ll) {
  return find_end(ll);
}

size_t linked_list_get_len(struct linked_list *ll) {
  return find_len(ll);
}

int linked_list_delete(struct linked_list *ll, size_t pos) {
  struct linked_list *obj = find_pos(ll, pos);
  if (obj == NULL) return 1;
  obj->prev->next = obj->next;
  obj->next->prev = obj->prev;
  linked_list_free(obj);
  return 0;
}

int linked_list_delete_node(struct linked_list *ll) {
  if (ll == NULL) return 1;
  if (ll->prev != NULL) {
    ll->prev->next = ll->next;
  }
  if (ll->next != NULL) {
    ll->next->prev = ll->prev;
  }
  linked_list_free(ll);
  return 0;
}

void linked_list_free(struct linked_list *ll) {
  free_line(&ll->value);
  ll->next = NULL;
  ll->prev = NULL;
  free(ll);
}

void linked_list_free_all(struct linked_list *ll) {
  struct linked_list *cur = ll;
  while(cur != NULL) {
    struct linked_list* n = cur->next;
    linked_list_free(cur);
    cur = n;
  }
}

