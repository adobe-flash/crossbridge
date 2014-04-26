/* APPLE LOCAL file opt diary */
/* Test optimization diary version */
/* { dg-assemble } */
/* { dg-options "-fopt-diary -dA -gdwarf-2" } */
void foo ()
{
}
/* { dg-final { scan-assembler "DW_AT_GNU_OD_version" } } */
