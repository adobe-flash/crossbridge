/* { dg-options "-O2 -fdump-tree-optimized -fwhole-program" } */
/* LLVM LOCAL test not applicable */
/* { dg-require-fdump "" } */
__attribute__ ((externally_visible))
void
externally_visible_function ()
{
}
/* { dg-final { scan-tree-dump "externally_visible_function" "optimized"} } */
/* { dg-final { cleanup-tree-dump "optimized" } } */
