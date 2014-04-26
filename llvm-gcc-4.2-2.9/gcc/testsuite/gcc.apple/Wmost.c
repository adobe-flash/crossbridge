/* APPLE LOCAL file -Wmost */

/* { dg-do compile } */
/* { dg-options "-Wmost" } */

int baz (void);

int
foo ()
{
  int loc;

  bar ();  /* { dg-warning "implicit declaration" } */

  if (loc = baz ())  /* be quiet about this */
    return 1;
  return 0;
}

