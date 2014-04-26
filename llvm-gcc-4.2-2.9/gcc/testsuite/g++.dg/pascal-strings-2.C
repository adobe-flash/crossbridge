/* APPLE LOCAL file pascal strings */
/* Negative C++ test cases.  */
/* Origin: Ziemowit Laski <zlaski@apple.com> */
/* { dg-do compile } */
/* { dg-options "-fpascal-strings" } */

const wchar_t *pascalStr1 = L"\pHi!"; /* { dg-bogus "unknown escape sequence" } */
const wchar_t *pascalStr2 = L"Bye\p!"; /* { dg-warning "unknown escape sequence" } */

const wchar_t *initErr0 = "\pHi";   /* { dg-error "cannot convert" } */
const wchar_t initErr0a[] = "\pHi";  /* { dg-error "initialized from non-wide string" } */
const wchar_t *initErr1 = "Bye";   /* { dg-error "cannot convert" } */
const wchar_t initErr1a[] = "Bye";   /* { dg-error "initialized from non-wide string" } */

const char *initErr2 = L"Hi";   /* { dg-error "cannot convert" } */
const char initErr2a[] = L"Hi";  /* { dg-error "initialized from wide string" } */
const signed char *initErr3 = L"Hi";  /* { dg-error "cannot convert" } */
const signed char initErr3a[] = L"Hi";  /* { dg-error "initialized from wide string" } */
const unsigned char *initErr4 = L"Hi";  /* { dg-error "cannot convert" } */
const unsigned char initErr4a[] = L"Hi"; /* { dg-error "initialized from wide string" } */

const char *pascalStr3 = "Hello\p, World!"; /* { dg-warning "unknown escape sequence" } */

const char *concat2 = "Hi" "\pthere"; /* { dg-warning "unknown escape sequence" } */
const char *concat3 = "Hi" "there\p"; /* { dg-warning "unknown escape sequence" } */

const char *s2 = "\pGoodbye!";   /* { dg-error "invalid conversion" } */
unsigned char *s3 = "\pHi!";     /* { dg-error "invalid conversion" } */
char *s4 = "\pHi";               /* { dg-error "invalid conversion" } */
signed char *s5 = "\pHi";        /* { dg-error "invalid conversion" } */
const signed char *s6 = "\pHi";  /* { dg-error "invalid conversion" } */

/* the maximum length of a Pascal literal is 255. */
const unsigned char *almostTooLong =
  "\p12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "123456789012345";  /* ok */
const unsigned char *definitelyTooLong =
  "\p12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    "1234567890123456";  /* { dg-error "too long" } */
