#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "common.h"
#include "demo.h"
#include "demo_dir.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

static int scan_item(struct dir_item *item)
{
    return (item->data = demo_load(item->path)) ? 1 : 0;
}

Array demo_dir_scan(void)
{
    return fs_dir_scan("Replays", scan_item);
}

void demo_dir_free(Array items)
{
    struct dir_item *item;
    int i;

    for (i = 0; i < array_len(items); i++)
    {
        item = array_get(items, i);

        demo_free(item->data);
        item->data = NULL;
    }

    dir_free(items);
}

/*---------------------------------------------------------------------------*/
