#include "config.h"
#include <stdio.h>
#include <stdbool.h>
#include "deps/tomlc99/toml.h"

void config_init(struct config *c) {
  c->leader = '\\';
  c->font_scale = 1;
  c->fps = 60.0;
  c->font_point = 48;
  c->win_width = 640;
  c->win_height = 480;
  c->read_size = BUFSIZ * 4;
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
  toml_datum_t font_scale = toml_double_in(main, "font_scale");
  if (font_scale.ok) {
    c->font_scale = font_scale.u.d;
  }
  toml_datum_t fps = toml_double_in(main, "fps");
  if (fps.ok) {
    c->fps = fps.u.d;
    if (c->fps >= 1000) c->fps = 1000;
  }
  toml_datum_t font_point = toml_int_in(main, "font_point");
  if (font_point.ok) {
    c->font_point = font_point.u.i;
  }
  toml_datum_t win_height = toml_int_in(main, "win_height");
  if (win_height.ok) {
    c->win_height = win_height.u.i;
  }
  toml_datum_t win_width = toml_int_in(main, "win_width");
  if (win_width.ok) {
    c->win_width = win_width.u.i;
  }
  toml_datum_t read_size = toml_int_in(main, "read_size");
  if (read_size.ok) {
    c->read_size = read_size.u.i;
    if (c->read_size == 0) c->read_size = BUFSIZ * 4;
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

