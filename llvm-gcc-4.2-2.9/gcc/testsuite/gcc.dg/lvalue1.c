/* PR c/5225 */
/* { dg-do compile } */
/* APPLE LOCAL non lvalue assign */
/* { dg-options "-fno-non-lvalue-assign" } */

int main()
{
  int i;
  +i = 1;	/* { dg-error "lvalue required as left operand of assignment" } */
  return 0;
}
