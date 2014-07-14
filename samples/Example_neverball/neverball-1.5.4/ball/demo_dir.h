#ifndef DEMO_SCAN
#define DEMO_SCAN

#include "demo.h"
#include "array.h"
#include "dir.h"

#define DEMO_GET(a, i) ((struct demo *) DIR_ITEM_GET((a), (i))->data)

Array demo_dir_scan(void);
void  demo_dir_free(Array);

#endif
