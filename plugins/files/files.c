#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <types/display_type.h>
#include <types/menu_types.h>
#include <types/plugin_interface_types.h>
#include <unistd.h>

#include "files.h"

struct dir_info {
  char *base;
  char *cur_dir;
  char *name;
  DIR *dir;
};

const char *prompt = "view files";
menu_item_array buffer;
struct dir_info dir_t = {
  .base = NULL,
  .cur_dir = NULL,
  .name = NULL,
  .dir = NULL,
};

struct message_t msg_payload = {
  .msg = NULL,
  .len = 0,
};

bool gen_items_from_dir(const char *wd, menu_item_array *buf);

static SDL_Color color_for_path_type(uint8_t d_type) {
  switch (d_type) {
    case DT_REG:
      return (SDL_Color){255,255,255,255};
    case DT_DIR:
      return (SDL_Color){17,123, 158, 255};
    case DT_LNK:
      return (SDL_Color){19,156, 61, 255};
    default:
      return (SDL_Color){200,40, 0, 255};
  }
}

bool setup(struct plugin_interface* pi) {
  size_t size = 1024;
  dir_t.base = malloc(sizeof(char) *size);
  char *ptr = getcwd(dir_t.base, size);
  for (; ptr == NULL; size *= 2) {
    if ((dir_t.base = realloc(dir_t.base, size)) == NULL) {
      fprintf(stderr, "allocation error\n.");
      return false;
    }
    ptr = getcwd(dir_t.base, size);
    if (ptr == NULL & errno != ERANGE) {
      fprintf(stderr, "getting current working directory failed.\n");
      return false;
    }
  }
  dir_t.cur_dir = dir_t.base;
  return true;
}

bool is_dir(const char *path) {
  struct stat stats;
  stat(path, &stats);
  return S_ISDIR(stats.st_mode);
}

bool file_selected(struct display *d, void *ctx) {
  char *filename = (char*)ctx;
  size_t cur_dir_len = strlen(dir_t.cur_dir);
  // add 2 for path separator and null-terminator
  char *full_path = (char*)malloc((sizeof(char) * (cur_dir_len + strlen(filename))) + 2);
  if (full_path == NULL) {
    fprintf(stderr, "malloc failed for path\n");
    return false;
  }
  sprintf(full_path, "%s%c%s", dir_t.cur_dir, '/', filename);
  if (is_dir(full_path)) {
    if (dir_t.base != dir_t.cur_dir) {
      free(dir_t.cur_dir);
    }
    // free previous path if it was not the base path
    dir_t.cur_dir = full_path;
    if (!gen_items_from_dir(full_path, &buffer)) {
      return false;
    }
    d->state.pi.dispatch(&d->state.pi, DISPATCH_MENU, &buffer);
    free_menu_item_array(&buffer);
  } else {
    d->state.pi.dispatch(&d->state.pi, DISPATCH_NEW_PAGE, full_path);
  }
  return true;
}

int file_compare(const void *a, const void *b) {
  struct menu_item *obj1 = (struct menu_item *)a;
  struct menu_item *obj2 = (struct menu_item *)b;
  return strcmp(obj1->name, obj2->name);
}

bool gen_items_from_dir(const char *wd, menu_item_array *buf) {
  if (wd == NULL) return false;
  if (dir_t.dir != NULL) {
    closedir(dir_t.dir);
    dir_t.dir = NULL;
  }
  struct dirent *dp = NULL;
  if ((dir_t.dir = opendir(wd)) == NULL) {
    fprintf(stderr, "failed to open directory %s\n", wd);
    return false;
  }
  do {
    if ((dp = readdir(dir_t.dir)) != NULL) {
      struct menu_item mi;
      // since we just reuse the dir's char pointer
      // we do not free the dir immediately
      mi.name = dp->d_name;
      mi.selected = file_selected;
      mi.ctx = dp->d_name;
      mi.use_color = true;
      mi.color = color_for_path_type(dp->d_type);
      insert_menu_item_array(&buffer, mi);
    }
  } while (dp != NULL);
  qsort(buffer.menu_item_data, buffer.len, sizeof(struct menu_item), file_compare);
  return true;
}

bool action(struct plugin_interface *d) {
  // reset cur_dir to base path
  if (dir_t.base != dir_t.cur_dir) {
    free(dir_t.cur_dir);
    dir_t.cur_dir = dir_t.base;
  }
  if (!gen_items_from_dir(dir_t.base, &buffer)) {
    return false;
  }
  // dispatch menu changes to COMMAND mode
  d->dispatch(d, DISPATCH_MENU, &buffer);
  free_menu_item_array(&buffer);
  return true;
}
void get_display_prompt(const char **out) {
  *out = prompt;
}

bool cleanup(struct plugin_interface* pi) {
  if (dir_t.dir != NULL) {
    closedir(dir_t.dir);
    dir_t.dir = NULL;
  }
  free(dir_t.base);
  if (buffer.menu_item_data != NULL) {
    free_menu_item_array(&buffer);
  }
  return true;
}
