/* APPLE LOCAL file __block assign sequence point 6722072 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

int i = 0;

int die() { return i++; }

int main() {
  __block int ret = 1;
  ret = die();
  return ret;
}
