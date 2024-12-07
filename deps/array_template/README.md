# Array Template

### TODOS
- write unit tests

---

A small header only C file to generate arrays of a given type.

To generate a template array for your type create your header file and include in `array_template.h`.

The following examples will be based on generating a template array for an `int` array with the name `int`.

Example:
```c
#ifndef INT_ARRAY_H
#define INT_ARRAY_H

#include "array_template.h"

/**
 * This function generates all of the template array functions and types.
 *
 * The first parameter is the name you want to give it. This will expand to
 * int_array.
 *
 * The second parameter is the type. So the internal array in the template
 * array struct will be `int *int_data`
 */
generate_array_template(int, int)

#endif
```

A struct is generated with the format:
```c
typedef struct int_array {
    size_t len;
    size_t cap;
    int *int_data;
} int_array;
```

The functions generated are (all functions are `static inline void`):
- Initializing the array.
```c
/**
 * Initialize the template array with a given size
 *
 * @param[in] arr The template array to get initialized
 * @param[in] N The size to allocate to
 */
init_int_array(int_array *arr, size_t N)
```

- Freeing the array.
```c
/**
 * Free the template array.
 * len and cap are set to 0.
 *
 * @param[in] arr The template array to free
 */
free_int_array(int_array *arr)
```

- Inserting items into the array. This function handles reallocating the array.
```c
/**
 * Insert an item into the template array.
 *
 * @param[in] arr The template array
 * @param[in] obj The item to insert
 */
insert_int_array(int_array *arr, int obj)
```

- Getting a value from the array.
```c
/**
 * Get an item from the template array.
 *
 * @param[in] arr The template array
 * @param[in] index The index to access
 * @param[out] out Set to the item at the index, if index out of bounds it's not
 * set.
 */
get_int_array(const int_array *arr, size_t index, int *obj)
```


