/* APPLE LOCAL file mainline */
/* { dg-do run } */

#include <stdlib.h>
#include <string.h>

template <class T>
struct Vec {
 T x, y;
 int  z;
 long long zz;
};

Vec<double> dd;
const char *enc = @encode(Vec<float>);
const char *enc2 = @encode(Vec<double>);

int main(void) {
  const char *encode = @encode(long);

  /* APPLE LOCAL begin ARM 5804096 */
  switch (sizeof (long))
    {
      case 4:
	if (strcmp (encode, "l"))
	  abort ();
	break;
      case 8:
	if (strcmp (encode, "q"))
	  abort ();
	break;
    }
  /* APPLE LOCAL end ARM 5804096 */

  if (strcmp (enc, "{Vec<float>=ffiq}"))
    abort();

  if (strcmp (enc2, "{Vec<double>=ddiq}"))
    abort();

  return 0;
}
