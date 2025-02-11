#include "sessions.h"
#include <stddef.h>
#include <stdio.h>

static size_t session_ids = 1;

bool session_add(struct session_array* arr, struct session s) {
  s.id = session_ids++;
  if (!insert_session_array(arr, s)) {
    return false;
  }
  return true;
}

bool session_get(struct session_array* arr, size_t id, struct session *out) {
  bool found = false;
  for (int i = 1; i < arr->len; ++i) {
    if (arr->session_data[i].id == id) {
      found = true;
      *out = arr->session_data[i];
      break;
    }
  }
  return found;
}

bool session_update(struct session_array* arr, struct session s) {
  bool found = false;
  for (int i = 1; i < arr->len; ++i) {
    if (arr->session_data[i].id == s.id) {
      found = true;
      arr->session_data[i] = s;
      break;
    }
  }
  return found;
}

bool session_remove(struct session_array* arr, struct session s) {
  bool found = false;
  for (int i = 0; i < arr->len; ++i) {
    if (arr->session_data[i].id == s.id) {
      found = true;
      arr->session_data[i] = arr->session_data[arr->len - 1];
      arr->len--;
      break;
    }
  }
  return found;
}

bool session_remove_idx(struct session_array* arr, size_t idx) {
  if (idx > arr->len) return false;
  arr->session_data[idx] = arr->session_data[arr->len - 1];
  arr->len--;
  return true;
}
