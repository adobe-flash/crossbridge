// APPLE LOCAL file radar 4646337
// { dg-do run { target *-*-darwin* } }

#include <cstdlib>

/* Test the size and alignment of fundamental C types for compliance
   with the IA-64 ABI.  */

enum A { a };

int main ()
{
  if (sizeof (char)                    !=  1)
    abort ();
  if (__alignof__ (char)               !=  1)
    abort ();
  if (sizeof (signed char)             !=  1)
    abort ();
  if (__alignof__ (signed char)        !=  1)
    abort ();
  if (sizeof (unsigned char)           !=  1)
    abort ();  
  if (__alignof__ (unsigned char)      !=  1)
    abort ();
  if (sizeof (short)                   !=  2)
    abort ();
  if (__alignof__ (short)              !=  2)
    abort ();
  if (sizeof (signed short)            !=  2)
    abort ();
  if (__alignof__ (signed short)       !=  2)
    abort ();
  if (sizeof (unsigned short)          !=  2)
    abort ();
  if (__alignof__ (unsigned short)     !=  2)
    abort ();
  if (sizeof (int)                     !=  4)
    abort ();
  if (__alignof__ (int)                !=  4)
    abort ();
  if (sizeof (signed int)              !=  4)
    abort ();
  if (__alignof__ (signed int)         !=  4)
    abort ();
  if (sizeof (unsigned int)            !=  4)
    abort ();
  if (__alignof__ (unsigned int)       !=  4)
    abort ();
#if defined(__x86_64__) || defined (__ppc64__)
  if (sizeof (long)                    !=  8)
    abort ();
  if (__alignof__ (long)               !=  8)
    abort ();
  if (sizeof (signed long)             !=  8)
    abort ();
  if (__alignof__ (signed long)        !=  8)
    abort ();
  if (sizeof (unsigned long)           !=  8)
    abort ();
  if (__alignof__ (unsigned long)      !=  8)
    abort ();
#else
  if (sizeof (long)                    !=  4)
    abort ();
  if (__alignof__ (long)               !=  4)
    abort ();
  if (sizeof (signed long)             !=  4)
    abort ();
  if (__alignof__ (signed long)        !=  4)
    abort ();
  if (sizeof (unsigned long)           !=  4)
    abort ();
  if (__alignof__ (unsigned long)      !=  4)
    abort ();
#endif /* __x86_64__ || __ppc64__ */
#if defined(__arm__)
  if (sizeof (long long)               !=  8)
    abort ();
  if (__alignof__ (long long)          !=  4)
    abort ();
  if (sizeof (signed long long)        !=  8)
    abort ();
  if (__alignof__ (signed long long)   !=  4)
    abort ();
  if (sizeof (unsigned long long)      !=  8)
    abort ();
  if (__alignof__ (unsigned long long) !=  4)
    abort ();
  if (sizeof (enum A)                  !=  4)
    abort ();
  if (__alignof__ (enum A)             !=  4)
    abort ();
#else
  if (sizeof (long long)               !=  8)
    abort ();
  if (__alignof__ (long long)          !=  8)
    abort ();
  if (sizeof (signed long long)        !=  8)
    abort ();
  if (__alignof__ (signed long long)   !=  8)
    abort ();
  if (sizeof (unsigned long long)      !=  8)
    abort ();
  if (__alignof__ (unsigned long long) !=  8)
    abort ();
  if (sizeof (enum A)                  !=  4)
    abort ();
  if (__alignof__ (enum A)             !=  4)
    abort ();
#endif /* __arm__ */
#ifdef HAVE_IA64_TYPES
  if (sizeof (__int64)                 !=  8)
    abort ();
  if (__alignof__ (__int64)            !=  8)
    abort ();
  if (sizeof (signed __int64)          !=  8)
    abort ();
  if (__alignof__ (signed ___int64)    !=  8)
    abort ();
  if (sizeof (unsigned __int64)        !=  8)
    abort ();
  if (__alignof__ (unsigned __int64)   !=  8)
    abort ();
  if (sizeof (__int128)                != 16)
    abort ();
  if (__alignof__ (__int128)           != 16)
    abort ();
  if (sizeof (signed __int128)         != 16)
    abort ();
  if (__alignof__ (signed ___int128)   != 16)
    abort ();
  if (sizeof (unsigned __int128)       != 16)
    abort ();
  if (__alignof__ (unsigned ___int128) != 16)
    abort ();
#endif  /* HAVE_IA64_TYPES  */
#if defined(__x86_64__) || defined (__ppc64__)
  if (sizeof (void *)                  !=  8)
    abort ();
  if (__alignof__ (void *)             !=  8)
    abort ();
  if (sizeof (void (*) ())             !=  8)
    abort ();
  if (__alignof__ (void (*) ())        !=  8)
    abort ();
#else
  if (sizeof (void *)                  !=  4)
    abort ();
  if (__alignof__ (void *)             !=  4)
    abort ();
  if (sizeof (void (*) ())             !=  4)
    abort ();
  if (__alignof__ (void (*) ())        !=  4)
    abort ();
#endif  /* __x86_64__ || __ppc64__ */
#if defined(__arm__)
  if (sizeof (float)                   !=  4)
    abort ();
  if (__alignof__ (float)              !=  4)
    abort ();
  if (sizeof (double)                  !=  8)
    abort();
  if (__alignof__ (double)             !=  4)
    abort();
  if (sizeof (long double)             !=  8)
    abort ();
  if (__alignof__ (long double)        !=  4)
    abort ();
#else
  if (sizeof (float)                   !=  4)
    abort ();
  if (__alignof__ (float)              !=  4)
    abort ();
  if (sizeof (double)                  !=  8)
    abort ();
  if (__alignof__ (double)             !=  8)
    abort ();
  if (sizeof (long double)             != 16)
    abort ();
  if (__alignof__ (long double)        != 16)
    abort ();
#endif /* __arm__ */
#ifdef HAVE_IA64_TYPES
  if (sizeof (__float80)               != 16)
    abort ();
  if (__alignof__ (__float80)          != 16)
    abort ();
  if (sizeof (__float128)              != 16)
    abort ();
  if (__alignof__ (__float128)         != 16)
    abort ();
#endif  /* HAVE_IA64_TYPES  */

  return 0;
}
