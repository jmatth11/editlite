#include "registry.h"
#include <stdio.h>

bool registry_init(struct registry *reg)  {
  if (!init_insert_func_array(&reg->insert_funcs, 2)) {
    fprintf(stderr, "error initializing registry insert_funcs.\n");
    return false;
  }
  if (!init_render_glyph_func_array(&reg->render_glyph_funcs, 2)) {
    fprintf(stderr, "error initializing registry render_glyph_funcs.\n");
    return false;
  }
  if (!init_page_change_func_array(&reg->page_change_funcs, 2)) {
    fprintf(stderr, "error initializing registry page_change_funcs.\n");
    return false;
  }
  return true;
}

bool registry_add(struct registry *reg, const struct registry_entry *entry) {
  if (entry->callback == NULL) {
    fprintf(stderr, "error entry's callback was null.\n");
    return false;
  }
  switch (entry->type) {
    case REG_INSERT: {
      insert_event e = (insert_event)entry->callback;
      if (!insert_insert_func_array(&reg->insert_funcs, e)) {
        fprintf(stderr, "error adding insert_func.\n");
        return false;
      }
      break;
    }
    case REG_RENDER_GLYPH: {
      render_glyph e = (render_glyph)entry->callback;
      if (!insert_render_glyph_func_array(&reg->render_glyph_funcs, e)) {
        fprintf(stderr, "error adding render_glyph_funcs.\n");
        return false;
      }
      break;
    }
    case REG_PAGE_CHANGE: {
      page_change e = (page_change)entry->callback;
      if (!insert_page_change_func_array(&reg->page_change_funcs, e)) {
        fprintf(stderr, "error adding page_change_funcs.\n");
        return false;
      }
      break;
    }
  }
  return true;
}

void registry_free(struct registry *reg) {
  free_insert_func_array(&reg->insert_funcs);
  free_render_glyph_func_array(&reg->render_glyph_funcs);
  free_page_change_func_array(&reg->page_change_funcs);
}
