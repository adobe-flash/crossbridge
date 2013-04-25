/* APPLE LOCAL kext weak_import 5935650 */
/* { dg-do assemble { target i?86*-*-darwin* } } */
/* { dg-options "-mkernel -m64" } */

extern void foo() __attribute__((weak_import));
void test() {
  if (foo)
    foo();
}
