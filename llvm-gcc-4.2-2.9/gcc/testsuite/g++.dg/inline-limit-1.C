/* APPLE LOCAL file 4209014 4210936 */
/* { dg-do compile } */
/* LLVM LOCAL -m32 */
/* { dg-options "-Os -m32" } */
/* Call to inlinex1 should be inlined.  */
/* { dg-final { scan-assembler-not "(\tcall|\tbl)\[ 	a-zA-Z0-9_\]*inlinex1" } } */
/* Call to calleex1 should be called, not inlined.  */
/* { dg-final { scan-assembler "(\tcall|\tbl)\[ 	a-zA-Z0-9_\]*calleex1" } } */

/* Insure that trivial callees (up to 30 "estimated insns") are
   inlined only if marked 'inline' in C++/Obj-C++.  It's unfortunate,
   but this testcase is expected to require revision every time the
   GCC inlining heuristics change.  */

extern void bulk (int);	/* CALLs are assumed to generate 10 'estimated insns'.  */

/* Inlining limits for inline and non-inline functions are currently
   identical for C++.  Empirically, a body with one will be inlined,
   and a body with two calls will not.  */

void
calleex1 (void)
{
  bulk (1); bulk (2);
}

void inline
inlinex1 (void)
{
  bulk (1);
}

int
main ()
{
  calleex1 ();
  inlinex1 ();
  return 0;
}
