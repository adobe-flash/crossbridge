/* APPLE LOCAL file radar 5732232, 6034839 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void foo(int x, int y);

int main() {
  ^(int x)foo(x, 4); /* { dg-error "blocks require" } */
  ^(int x, int y)foo(y, x); /* { dg-error "blocks require" } */
  ^(int x)(x+4); /* { dg-error "blocks require" } */
  ^(z+4); /* { dg-error "expected" } */
}

