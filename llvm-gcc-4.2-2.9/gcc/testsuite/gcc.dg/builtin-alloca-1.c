/* APPLE LOCAL file radar 5155743, mainline candidate */
/* { dg-do run } */
/* PR target/28197 */

#include <setjmp.h>

jmp_buf env;
void f (int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{
  longjmp (env, 1);
}

int main()
{
  if (setjmp (env) == 0) {
    void *p = __builtin_alloca (4);
    f (1,2,3,4,5,6,7,8);
  }
  return 0;
}
