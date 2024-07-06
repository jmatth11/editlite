#ifndef EDITLITE_CONFIG_H
#define EDITLITE_CONFIG_H

struct config;

void config_init(struct config *c);
void parse_config(struct config *c);
void config_free(struct config *c);

#endif
