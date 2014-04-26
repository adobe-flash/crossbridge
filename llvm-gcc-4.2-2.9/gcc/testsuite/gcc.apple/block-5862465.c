/* APPLE LOCAL file 5862465 */
/* { dg-do compile } */
/* { dg-options "-O1" } */
void enumerateObjectsWithOptions(unsigned int x, void (^block)(char *obj, unsigned int idx, signed char *stop))
{
  signed char stop = (signed char)0;
  char *item;
  unsigned int idx = 10;
  for ( ; idx > 0 ; idx--)
    block(item, idx, &stop);
}
