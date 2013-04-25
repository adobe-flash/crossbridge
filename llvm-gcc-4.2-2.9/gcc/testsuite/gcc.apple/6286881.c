/* APPLE LOCAL file radar 6286881 */
/* Check for incorrect expression does not result in compiler looping; instead of diagnostic. */
/* { dg-do compile } */
unsigned int
func_48 (signed char p_49)
{
  signed char l_340;
  func_44 (&((1 ^ 1 == (lshift_u_s (1)) != (l_340 < 1)) & 1L));  /* { dg-error "lvalue required as unary" } */
}
