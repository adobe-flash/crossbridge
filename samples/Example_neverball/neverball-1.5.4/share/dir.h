#ifndef DIR_H
#define DIR_H

#include "array.h"

struct dir_item
{
    const char *path;
    void *data;
};

/*
 * A minor convenience for quick member access, as in "DIR_ITEM_GET(a,
 * i)->member".  Most of the time this macro is not what you want to
 * use.
 */
#define DIR_ITEM_GET(a, i) ((struct dir_item *) array_get((a), (i)))

Array dir_scan(const char *,
               int    (*filter)   (struct dir_item *),
               char **(*get_list) (const char *),
               void   (*free_list)(void *));
void  dir_free(Array);

#endif
