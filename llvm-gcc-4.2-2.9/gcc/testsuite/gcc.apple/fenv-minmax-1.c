/* APPLE LOCAL file Radar 3943021 */
/* { dg-do compile { target powerpc-*-darwin* } } */
/* { dg-options "-ffast-math" } */
#pragma GCC fenv
float size[2], tex_size;
void
test_of_pragma_fenv ()
{
  size[0] = ((size[0]) < (tex_size) ? (size[0]) : (tex_size));
}

