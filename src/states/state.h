#ifndef EDITLITE_STATE_H
#define EDITLITE_STATE_H

#include <stdbool.h>

struct app_state;
struct display_dim;
struct page;

bool state_init(struct app_state*);
bool state_load_plugins(struct app_state*);
bool state_reload_plugins(struct app_state*);
bool state_get_cur_page(struct app_state*, struct page **out);
void state_get_page_dim(struct app_state*, struct display_dim *out);
void state_free(struct app_state*);

#endif
