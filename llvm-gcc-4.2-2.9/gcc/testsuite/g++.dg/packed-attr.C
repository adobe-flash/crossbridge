/* APPLE LOCAL file radar 4121962 */
/* { dg-do compile } */
struct foo
{
  foo () {
    bar[0] = 0;
  }
  char bar[32];
} __attribute__ ((aligned (2), packed));

struct foos
{
  foo descriptors[1];
} __attribute__ ((aligned (2), packed));
/* APPLE LOCAL file radar 4121962 */
