/* APPLE LOCAL file pointer casts */
/* Test that casts of pointer to unsigned long long aren't sign extended */
/* Author: Matt Austern <austern@apple.com> */
/* { dg-do run } */
/* { dg-options "-Wno-error -w" } */

int main () {
  /* Note: test assumes sizeof(long long) >= sizeof(void*) */

  unsigned long x1 = 0x80000000ul;
  void* p = (void*) x1;
  unsigned long long x2 = (unsigned long long) p;

  return !(x1 == x2);
}
