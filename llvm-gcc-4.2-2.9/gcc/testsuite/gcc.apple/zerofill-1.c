/* { dg-do compile { target *-*-darwin* } } */
/* { dg-final { scan-assembler "foo,bar" } } */

int foo __attribute__((section("foo,bar")));
