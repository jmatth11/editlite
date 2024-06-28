#include "files.h"
#include <page.h>
#include <display.h>
#include <menu.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

const char *prompt = "view files";
menu_item_array buffer;
char *cwd = NULL;

bool gen_items_from_dir(const char *wd, menu_item_array *buf);

bool setup(struct plugin_interface* pi) {
  size_t size = 1024;
  cwd = malloc(sizeof(char) *size);
  char *ptr = getcwd(cwd, size);
  for (; ptr == NULL; size *= 2) {
    if ((cwd = realloc(cwd, size)) == NULL) {
      fprintf(stderr, "allocation error\n.");
      return false;
    }
    ptr = getcwd(cwd, size);
    if (ptr == NULL & errno != ERANGE) {
      fprintf(stderr, "getting current working directory failed.\n");
      return false;
    }
  }
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
  // TODO handle more than top level directory
  if (is_dir(filename)) {
    if (!gen_items_from_dir(filename, &buffer)) {
      return false;
    }
    d->pi.dispatch(&d->pi, DISPATCH_MENU, &buffer);
    free_menu_item_array(&buffer);
  } else {
    d->pi.dispatch(&d->pi, DISPATCH_NEW_PAGE, filename);
  }
  return true;
}

int file_compare(const void *a, const void *b) {
  struct menu_item *obj1 = (struct menu_item *)a;
  struct menu_item *obj2 = (struct menu_item *)b;
  return strcmp(obj1->name, obj2->name);
}

bool gen_items_from_dir(const char *wd, menu_item_array *buf) {
  if (cwd == NULL) return false;
  DIR *dir;
  struct dirent *dp = NULL;
  if ((dir = opendir(cwd)) == NULL) {
    fprintf(stderr, "failed to open directory %s\n", cwd);
    return false;
  }
  do {
    if ((dp = readdir(dir)) != NULL) {
      struct menu_item mi;
      mi.name = dp->d_name;
      mi.selected = file_selected;
      mi.ctx = dp->d_name;
      insert_menu_item_array(&buffer, mi);
    }
  } while (dp != NULL);
  qsort(buffer.menu_item_data, buffer.len, sizeof(struct menu_item), file_compare);
  //closedir(dir);
  return true;
}

bool action(struct plugin_interface *d) {
  if (!gen_items_from_dir(cwd, &buffer)) {
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
  free(cwd);
  if (buffer.menu_item_data != NULL) {
    free_menu_item_array(&buffer);
  }
  return true;
}
