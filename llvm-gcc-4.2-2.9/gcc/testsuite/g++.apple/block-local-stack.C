/* APPLE LOCAL file radar 5732232 - radar 6230297 - blocks */
/* { dg-do compile } */
/* { dg-options "-fblocks" } */

void (^test3())(void) {
  __block int i;
  return ^{i = 1; };   /* { dg-error "returning block that lives on the local stack" } */
}

