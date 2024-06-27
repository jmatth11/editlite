#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include "deps/tomlc99/toml.h"

void config_init(struct config *c) {
  c->leader = '\\';
  c->font_size = 14;
  c->cursor_color = (SDL_Color){0xAA,0xAA,0xAA,0x77};
  c->font_color = (SDL_Color){0xFF,0xFF,0xFF,0xFF};
  c->font_file = "resources/RobotoMono-Regular.ttf";
  if (init_string_array(&c->plugins, 1) != 0) {
    fprintf(stderr, "plugin array could not be initialized.\n");
  }
  insert_string_array(&c->plugins, "plugins/save/libsave.so");
  insert_string_array(&c->plugins, "plugins/quit/libquit.so");
}

void parse_config(struct config *c) {
  FILE *fp = fopen(CONFIG_FILENAME, "r+");
  if (fp == NULL) {
    return;
  }
  char errbuff[200];
  toml_table_t *conf = toml_parse_file(fp, errbuff, sizeof(errbuff));
  fclose(fp);
  if (!conf) {
    fprintf(stderr, "error parsing toml file: \"%s\"\n", errbuff);
    return;
  }
  toml_table_t *main = toml_table_in(conf, "main");
  if (!main) {
    fprintf(stderr, "[main] missing\n");
    return;
  }
  toml_datum_t leader = toml_string_in(main, "leader");
  if (leader.ok) {
    c->leader = leader.u.s[0];
  }
  toml_datum_t font_size = toml_int_in(main, "font_size");
  if (font_size.ok) {
    c->font_size = font_size.u.i;
  }
  toml_array_t *cursor_color = toml_array_in(main, "cursor_color");
  if (cursor_color) {
    toml_datum_t r = toml_int_at(cursor_color, 0);
    toml_datum_t g = toml_int_at(cursor_color, 1);
    toml_datum_t b = toml_int_at(cursor_color, 2);
    toml_datum_t a = toml_int_at(cursor_color, 3);
    if (r.ok) c->cursor_color.r = r.u.i;
    if (g.ok) c->cursor_color.g = g.u.i;
    if (b.ok) c->cursor_color.b = b.u.i;
    if (a.ok) c->cursor_color.a = a.u.i;
  }
  toml_array_t *font_color = toml_array_in(main, "font_color");
  if (font_color) {
    toml_datum_t r = toml_int_at(font_color, 0);
    toml_datum_t g = toml_int_at(font_color, 1);
    toml_datum_t b = toml_int_at(font_color, 2);
    toml_datum_t a = toml_int_at(font_color, 3);
    if (r.ok) c->font_color.r = r.u.i;
    if (g.ok) c->font_color.g = g.u.i;
    if (b.ok) c->font_color.b = b.u.i;
    if (a.ok) c->font_color.a = a.u.i;
  }
  toml_datum_t font_file = toml_string_in(main, "font_file");
  if (font_file.ok) c->font_file = font_file.u.s;
  toml_array_t *plugins = toml_array_in(main, "plugins");
  if (plugins) {
    for (int i = 0; ; ++i) {
      toml_datum_t p = toml_string_at(plugins, i);
      if (!p.ok) break;
      insert_string_array(&c->plugins, p.u.s);
    }
  }
  toml_free(conf);
}

