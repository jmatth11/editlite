#include "array_template.h"

#include "hash_map.h"
#include "types/array_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct hash_map_entry {
  code_point_t key;
  glyph_array value;
};

generate_array_template(map_entry, struct hash_map_entry *)

generate_array_template(map, map_entry_array)

struct hash_map {
  map_array entries;
};

static bool array_remove_glyph_item(glyph_array *arr, size_t idx) {
  if (idx >= arr->len || idx < 0) return false;
  if (idx == (arr->len -1)) {
    arr->len = arr->len - 1;
    return true;
  }
  // move item at index to end of array
  struct glyph_info local = arr->glyph_data[arr->len - 1];
  arr->glyph_data[arr->len - 1] = arr->glyph_data[idx];
  arr->glyph_data[idx] = local;
  // decrement the length to "remove" it
  arr->len = arr->len - 1;
  return true;
}

static int fast_mod(int hash, int cap) {
  // fast approximate modulo-like operation
  int even_cap = (1 & cap) ? (cap +1) : cap;
  int mod = (hash & even_cap);
  mod += (1 & hash);
  if (mod >= cap) mod -= cap;
  return mod;
}

static bool hash_map_remove_entry(map_entry_array *arr, size_t idx) {
  if (idx >= arr->len || idx < 0) return false;
  if (idx == (arr->len - 1)) {
    --arr->len;
    return true;
  }
  // move item at index to end of array
  arr->map_entry_data[idx] = arr->map_entry_data[arr->len - 1];
  arr->map_entry_data[arr->len - 1] = NULL;
  // decrement the length to "remove" it
  --arr->len;
  return true;
}

static bool hash_map_add_new_entry(map_entry_array *row, const code_point_t key, const struct glyph_info value) {
  struct hash_map_entry *entry = malloc(sizeof(struct hash_map_entry));
  entry->key = key;
  if (!init_glyph_array(&entry->value, 3)) {
    fprintf(stderr, "hash map entry array failed.\n");
    free(entry);
    return false;
  }
  if (!insert_glyph_array(&entry->value, value)) {
    fprintf(stderr, "insert value in new hash map failed.\n");
    free_glyph_array(&entry->value);
    free(entry);
    return false;
  }
  if (!insert_map_entry_array(row, entry)) {
    fprintf(stderr, "inserting new entry in hash map failed.\n");
    free_glyph_array(&entry->value);
    free(entry);
    return false;
  }
  return true;
}

struct hash_map * hash_map_create(size_t N) {
  struct hash_map *hm = malloc(sizeof(struct hash_map));
  if (hm == NULL) {
    fprintf(stderr, "error hash map init.\n");
    return NULL;
  }
  if (!init_map_array(&hm->entries, N)) {
    free(hm);
    fprintf(stderr, "error hash map init entries.\n");
    return NULL;
  }
  hm->entries.len = N;
  for (int i = 0; i < N; ++i) {
    hm->entries.map_data[i].map_entry_data = NULL;
    hm->entries.map_data[i].len = 0;
  }
  return hm;
}

void hash_map_destroy(struct hash_map *hm) {
  for (int i = 0; i < hm->entries.cap; ++i) {
    map_entry_array map_entry;
    get_map_array(&hm->entries, i, &map_entry);
    if (map_entry.len > 0) {
      for (int entry_idx = 0; entry_idx < map_entry.len; ++entry_idx) {
        struct hash_map_entry *entry = NULL;
        get_map_entry_array(&map_entry, entry_idx, &entry);
        if (entry != NULL) {
          const size_t val_len = entry->value.len;
          for (int val_idx = 0; val_idx < val_len; ++val_idx) {
            SDL_DestroyTexture(entry->value.glyph_data[val_idx].glyph);
          }
          free_glyph_array(&entry->value);
          free(entry);
        }
      }
      free_map_entry_array(&map_entry);
    }
  }
  free_map_array(&hm->entries);
  free(hm);
}

bool hash_map_get(struct hash_map *hm, const code_point_t key, glyph_array *out) {
  int hash = key;
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL || row->len == 0) {
    return false;
  }
  bool result = false;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *entry = NULL;
    get_map_entry_array(row, i, &entry);
    if (entry != NULL) {
      if (entry->key == key) {
        *out = entry->value;
        result = true;
        break;
      }
    }
  }
  return result;
}

bool hash_map_set(struct hash_map *hm, const code_point_t key, struct glyph_info value) {
  bool result = true;
  int hash = key;
  int idx = fast_mod(hash, hm->entries.cap);
  bool exists = false;
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    if (!init_map_entry_array(row, 10)) {
      fprintf(stderr, "error init map entry in hash map get.\n");
      result = false;
    }
  } else {
    for (int i = 0; i < row->len; ++i) {
      struct hash_map_entry *existing_entry = NULL;
      get_map_entry_array(row, i, &existing_entry);
      if (existing_entry != NULL && existing_entry->key == key) {
        exists = true;
        if (!insert_glyph_array(&existing_entry->value, value)) {
          fprintf(stderr, "insert value in existing hash map failed.\n");
          result = false;
        }
        break;
      }
    }
  }
  if (result && !exists) {
    if (!hash_map_add_new_entry(row, key, value)) {
      result = false;
    }
  }
  return result;
}

bool hash_map_remove(struct hash_map* hm, const code_point_t key) {
  int hash = key;
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    return false;
  }
  int remove_idx = -1;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && existing_entry->key == key) {
      remove_idx = i;
      const size_t val_len = existing_entry->value.len;
      for (int val_idx = 0; val_idx < val_len; ++val_idx) {
        SDL_DestroyTexture(existing_entry->value.glyph_data[val_idx].glyph);
      }
      free_glyph_array(&existing_entry->value);
      free(existing_entry);
      break;
    }
  }
  bool result = false;
  if (remove_idx != -1) {
    result = true;
    hash_map_remove_entry(row, remove_idx);
  }
  return result;
}

bool hash_map_remove_value(struct hash_map* hm, const code_point_t key, const struct glyph_info value) {
  int hash = key;
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    fprintf(stderr, "key does not exist.\n");
    return false;
  }
  int remove_idx = -1;
  bool result = true;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && existing_entry->key == key) {
      for (int entry_idx = 0; entry_idx < existing_entry->value.len; ++entry_idx) {
        struct glyph_info* local_val = &existing_entry->value.glyph_data[entry_idx];
        if (local_val->point == value.point) {
          remove_idx = entry_idx;
          SDL_DestroyTexture(local_val->glyph);
          break;
        }
      }
      if (remove_idx == -1) {
        fprintf(stderr, "hash map remove_value idx not found.\n");
        result = false;
      }
      if (!array_remove_glyph_item(&existing_entry->value, remove_idx)) {
        fprintf(stderr, "failed to remove int from hash map entry.\n");
        result = false;
      }
      break;
    }
  }
  return result;
}

