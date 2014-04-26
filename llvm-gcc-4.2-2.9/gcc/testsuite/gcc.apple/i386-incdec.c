/* APPLE LOCAL file radar 4577796 */
/* { dg-do compile { target "i?86-*-*" } } */
/* { dg-final { scan-assembler "inc" } } */
/* { dg-final { scan-assembler "dec" } } */
/* { dg-final { scan-assembler-not "addl\t\$1" } } */
/* { dg-final { scan-assembler-not "decl\t\$1" } } */
#define ARRAY_SIZE 16 
int foo()
{
   int x, t = 0;
   int a[ARRAY_SIZE];
   for (x = 0; x < ARRAY_SIZE; x++)
     t = t + a[x] + x;
   for (x = ARRAY_SIZE - 1; x >= 0; x--)
     t = t + a[x] + x;
   return t;
}
/* APPLE LOCAL file radar 4577796 */
