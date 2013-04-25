/* Test dead code strip support.  */
/* Contributed by Devang Patel  <dpatel@apple.com>  */
 
/* { dg-do compile { target *-*-darwin* } } */

const char my_version_string[] __attribute__((__used__))
  = "Do not remove this string\n";
 
 static int
 __attribute__((__used__))
      static_debug_routine()
{
   int i;
   i = 42;
}
 
int
main ()
{
   return 0;
}
/* LLVM LOCAL begin accept variant syntax */ 
/* { dg-final { scan-assembler ".no_dead_strip( |\t)_my_version_string" } } */
/* { dg-final { scan-assembler ".no_dead_strip( |\t)_static_debug_routine" } } */
/* LLVM LOCAL end */
