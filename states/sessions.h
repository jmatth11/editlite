#ifndef EDITLITE_SESSIONS_H
#define EDITLITE_SESSIONS_H

#include <stdbool.h>
#include <sys/cdefs.h>

#include "types/session_types.h"

/**
 * Add a session.
 * Function populates the ID property.
 */
bool session_add(struct session_array* arr, struct session s) __nonnull((1));
/**
 * Get the session with the given index.
 */
bool session_get(struct session_array* arr, size_t id, struct session* out) __nonnull((1));
/**
 * Update an existing session with the given session.
 * This function updates the session with the matching ID in the given session.
 */
bool session_update(struct session_array* arr, struct session s) __nonnull((1));
/**
 * Remove a session that matches the given session.
 */
bool session_remove(struct session_array* arr, struct session s) __nonnull((1));
/**
 * Remove a session that is at the given index.
 */
bool session_remove_idx(struct session_array* arr, size_t idx) __nonnull((1));

#endif
