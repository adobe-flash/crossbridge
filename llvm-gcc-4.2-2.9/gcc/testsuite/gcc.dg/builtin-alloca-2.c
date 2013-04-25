/* APPLE LOCAL file radar 5155743, mainline candidate */
/* { dg-do run } */

#include <setjmp.h>

jmp_buf env;
int main()
{
  char *p;
  int i;
  if (setjmp (env) == 0) {
    p = __builtin_alloca (1024);
    for (i = 0; i < 1024; i++) {
      p[i] = 0;
    }
    longjmp (env, 1);
  }
  return 0;
}
