#include "types.h"

/* add more platforms if necessary */

#ifdef UNIX

int64 millisecs()
{
  static int64 Time = 0;
  return ++Time; /* DO NOT TIME FOR LLVM */
}

#endif
