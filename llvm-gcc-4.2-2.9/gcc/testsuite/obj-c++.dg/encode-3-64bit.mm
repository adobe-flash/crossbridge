/* APPLE LOCAL file radar 4492973 */
/* Encoding in -m64 bit mode. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -m64" } */

#include <stdlib.h>
#include <string.h>

template <class T>
struct Vec {
 T x, y;
 long z;
 long long zz;
};

Vec<double> dd;
const char *enc = @encode(Vec<float>);
const char *enc2 = @encode(Vec<double>);

int main(void) {
  const char *encode = @encode(long);

  if (strcmp (encode, "q"))
    abort();

  if (strcmp (enc, "{Vec<float>=ffqq}"))
    abort();

  if (strcmp (enc2, "{Vec<double>=ddqq}"))
    abort();

  return 0;
}
