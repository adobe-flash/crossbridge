/* APPLE LOCAL file radar 5834569 */
/* Do not issue unused variable warning on blocks. */
/* { dg-do compile } */
/* { dg-options "-Wall" } */

int main() {
  __block int x = 10;
  int y = 1;

  int (^myBlock)(void) = ^{ return x+y; };

  myBlock();

  return 0;
}
