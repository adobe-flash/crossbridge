#include <stdlib.h>
#include <stdio.h>

/* declare all externally visible functions in libweak.c */
int dy_foo (char *, int);

main ()
{
  int answer, x=41;
  char *str = "foostr";
  printf ("%s begins:\n", __FILE__);
  answer = dy_foo (str, x);
  printf ("dy_foo (\"%s\", %d) = %d", str, x, answer);
  if (answer != 42)
    printf (" (error!)");
  printf ("\n%s done.\n", __FILE__);
  exit (answer != 42);
}
