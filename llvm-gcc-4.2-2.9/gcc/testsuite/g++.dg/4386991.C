/* APPLE LOCAL file 4386991 */
/* { dg-do run } */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* "BarAnimationData" and these fieldnames are historical, from
   HIToolbox where this originally broke.  */
struct BarAnimationData
{
  int cid;
  int window;
  int startTime;
  int totalTime;
  float percentDone;
  int *startBounds;
  int *endBounds;
  int *screenBounds;
  int done;
};

void spook_optimizer (void *p) __attribute__ ((__noinline__));
void
spook_optimizer (void *p)
{
  printf ("", p);	/* Try to fool the optimizer escape analysis.  */
}


int GetCGSConnectionID(void) __attribute__ ((__noinline__));
int
GetCGSConnectionID(void)
{
  return 42;
}

/* Test for complete initialization.  "AnimateBar" is historical;
   it's from HIToolbox, where this originally broke.  */
void AnimateBar (int) __attribute__ ((__noinline__));
void AnimateBar (int window)
{
  /* Initialize only the first two fields.  */
  BarAnimationData data = { GetCGSConnectionID(), window };
  /* Frighten the optimizer away.  */
  spook_optimizer ((void *)&data);
  if (data.startTime) abort ();
  if (data.totalTime) abort ();
  if (data.percentDone) abort ();
  if (data.startBounds) abort ();
  if (data.endBounds) abort ();
  if (data.screenBounds) abort ();
  if (data.done) abort ();
}

/* Clobber the local stack area with 0xffs.  */
void smear_stack () __attribute__ ((__noinline__));
void
smear_stack (void)
{
  char ary[400];
  memset (&ary, 0xff, sizeof (ary));
  spook_optimizer ((void *)&ary);
}

int
main ()
{
  smear_stack ();
  AnimateBar (42);
  return 0;
}
