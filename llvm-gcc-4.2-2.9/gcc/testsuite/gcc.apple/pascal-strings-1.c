/* APPLE LOCAL file pascal strings */
/* Positive C test cases.  */
/* Origin: Ziemowit Laski <zlaski@apple.com> */
/* { dg-do run } */
/* { dg-options "-std=iso9899:1999 -fpascal-strings" } */

typedef __WCHAR_TYPE__ wchar_t;
typedef __SIZE_TYPE__ size_t;

extern void abort (void);
extern size_t strlen (const char *s);

const unsigned char *pascalStr1 = "\pHello, World!";
const unsigned char *concat1 = "\pConcatenated" "string" "\pliteral";

const unsigned char msg1[] = "\pHello";   /* ok */
const unsigned char *msg2 = "\pHello";    /* ok */
const signed char msg3[] = "\pHello";     /* ok */
const char msg4[] = "\pHello";            /* ok */
unsigned char msg5[] = "\pHello";   /* ok */
signed char msg7[] = "\pHello";     /* ok */
char msg8[] = "\pHello";            /* ok */
   
int
main (void)
{
  const unsigned char *pascalStr2 = "\pGood-bye!";

  if (strlen ((const char *)pascalStr1) != 14)
    abort ();
  if (*pascalStr1 != 13)
    abort ();  /* the length byte does not include trailing null */

  if (strlen ((const char *)pascalStr2) != 10)
    abort ();
  if (*pascalStr2 != 9)
    abort ();

  if (strlen ((const char *)concat1) != 26)
    abort ();
  if (*concat1 != 25)
    abort ();

  return 0;
}

