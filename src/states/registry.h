#ifndef EDITLITE_REGISTRY_H
#define EDITLITE_REGISTRY_H

#include <stdbool.h>
#include <sys/cdefs.h>

#include "types/registered_functions.h"

__BEGIN_DECLS

bool registry_init(struct registry *reg) __THROWNL __nonnull((1));
bool registry_add(struct registry *reg, const struct registry_entry *entry) __THROWNL __nonnull((1,2));
void registry_free(struct registry *reg) __THROWNL __nonnull((1));

__END_DECLS

#endif
