#include "files.h"
#include <page.h>
#include <display.h>
#include <menu.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

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
bool gen_items_from_dir(const char *wd, menu_item_array *buf);

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
  if (S_ISDIR(stats.st_mode)) return true;
  return false;
}

bool file_selected(struct display *d, void *ctx) {
  char *filename = (char*)ctx;
  size_t cur_dir_len = strlen(dir_t.cur_dir);
  char *full_path = malloc(sizeof(char) * (cur_dir_len + strlen(filename) + 1));
  sprintf(full_path, "%s%c%s", dir_t.cur_dir, '/', filename);
  // TODO handle more than top level directory
  if (is_dir(full_path)) {
    // free previous path if it was not the base path
    if (dir_t.base != dir_t.cur_dir) {
      free(dir_t.cur_dir);
    }
    dir_t.cur_dir = full_path;
    if (!gen_items_from_dir(full_path, &buffer)) {
      return false;
    }
    d->pi.dispatch(&d->pi, DISPATCH_MENU, &buffer);
    free_menu_item_array(&buffer);
  } else {
    d->pi.dispatch(&d->pi, DISPATCH_NEW_PAGE, full_path);
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
