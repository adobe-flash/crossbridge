/* APPLE LOCAL file 4209014 4210936 */
/* { dg-do compile { target { powerpc*-*-darwin* i?86*-*-darwin* } } } */
/* { dg-options "-Os" } */
/* Call to inlinex25 should be inlined.  */
/* { dg-final { scan-assembler-not "(\tbl|\tcall)\[ 	\]*_?inlinex25" } } */
/* Call to calleex25 should be called, not inlined.  */
/* { dg-final { scan-assembler "(\tcall|\tbl)\[	 \]*_?calleex25" } } */

/* Insure non-trivial callees (between 30 and 450 "estimated insns")
   are inlined only if marked 'inline' in C/Obj-C.  It's unfortunate,
   but this testcase is expected to require revision every time the
   GCC inlining heuristics change.  */

extern void bulk (int);	/* CALLs are assumed to generate 10 'estimated insns'.  */

void
calleex25 (void)
{
  bulk ( 1); bulk ( 2); bulk ( 3); bulk ( 4); bulk ( 5);
  bulk ( 6); bulk ( 7); bulk ( 8); bulk ( 9); bulk (10);
  bulk (11); bulk (12); bulk (13); bulk (14); bulk (15);
  bulk (16); bulk (17); bulk (18); bulk (19); bulk (20);
  bulk (21); bulk (22); bulk (23); bulk (24); bulk (25);
}

void inline
inlinex25 (void)
{
  bulk ( 1); bulk ( 2); bulk ( 3); bulk ( 4); bulk ( 5);
  bulk ( 6); bulk ( 7); bulk ( 8); bulk ( 9); bulk (10);
  bulk (11); bulk (12); bulk (13); bulk (14); bulk (15);
  bulk (16); bulk (17); bulk (18); bulk (19); bulk (20);
  bulk (21); bulk (22); bulk (23); bulk (24); bulk (25);
}

int
main ()
{
  calleex25 ();
  inlinex25 ();
  return 0;
}
