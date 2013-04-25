/* APPLE LOCAL file 5799099 */
/* { dg-do compile } */
/* { dg-options "-O" } */
typedef struct CGSZone {
  void *context;
  void (*free)(void *context, void *ptr);
} CGSZone;

static void
CGSZoneFree(CGSZone *zone, void *ptr)
{
  (*zone->free)(zone->context, ptr);
}

void
release_bitmap_data(void *info, const void *data)
{
  /* The compiler should not warn when we explicitly cast away "const"
     here: */
  CGSZoneFree(info, (void *)data);
}
