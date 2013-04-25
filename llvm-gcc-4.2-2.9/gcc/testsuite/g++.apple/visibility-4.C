// APPLE LOCAL file 6983171 */
/* { dg-require-visibility "" } */
/* { dg-options "-fvisibility-ms-compat" } */

int foo() {
  try {
    throw (int*)0;
  } catch (...) {
    return 0;
  }

  return 1;
}
