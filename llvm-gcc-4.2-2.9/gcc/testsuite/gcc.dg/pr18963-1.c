/* { dg-do run } */
/* APPLE LOCAL testsuite nested functions */
/* { dg-options "-fnested-functions" } */

static int barf ();

int foo ()
{ 
  auto int barf ();
  int j = 4;

  int barf () {
    return j;
  }

  return barf ();
}

static int barf () {
  return 3;
}

extern void exit (int);
extern void abort ();

int main (int argc, char *argv[]) {
  if (foo () != 4)
    abort ();
  exit (0);
}
