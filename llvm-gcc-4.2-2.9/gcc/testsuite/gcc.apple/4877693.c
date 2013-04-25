/* { dg-error "-mstackrealign not supported in the 64bit mode" }
/* APPLE LOCAL file radar 4877693 */
/* { dg-do compile { target i?86-*-* } } */
/* { dg-options "-mstackrealign -m64" } */
int main (int argc, char **argv)
{
  return 0;
}
