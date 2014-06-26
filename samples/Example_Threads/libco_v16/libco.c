/*
  libco
  auto-selection module
  license: public domain
*/

#include "AS3/AS3.h"

int main()
{
    // We still need a main function for the SWC. this function must be called
    // so that all the static init code is executed before any library functions
    // are used.
    //
    // The main function for a library must throw an exception so that it does
    // not return normally. Returning normally would cause the static
    // destructors to be executed leaving the library in an unuseable state.

    AS3_GoAsync();
} 

#if defined(__AVM2__)
  #include "x86.c"
#elif defined(__GNUC__) && defined(__i386__)
  #include "x86.c"
#elif defined(__GNUC__) && defined(__amd64__)
  #include "amd64.c"
#elif defined(__GNUC__) && defined(_ARCH_PPC)
  #include "ppc.c"
#elif defined(__GNUC__)
  #include "sjlj.c"
#elif defined(_MSC_VER) && defined(_M_IX86)
  #include "x86.c"
#elif defined(_MSC_VER) && defined(_M_AMD64)
  #include "amd64.c"
#elif defined(_MSC_VER)
  #include "fiber.c"
#else
  #error "libco: unsupported processor, compiler or operating system"
#endif
