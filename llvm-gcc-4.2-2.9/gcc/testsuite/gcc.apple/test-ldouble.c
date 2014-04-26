/* { dg-do compile } */
/* { dg-options "-Os" } */

extern void __inline_x80told(long double *x);
extern double rint( double );

extern unsigned long AECreateDesc(const void * dataPtr);

unsigned long aeCoercePtrToMagnitude()
{
 unsigned long err = 0;
 unsigned long magValue;
 double doubleValue;
 long double longDoubleValue;

   __inline_x80told(&longDoubleValue);
   doubleValue = longDoubleValue;
   if (doubleValue < 0)
    err = 2;
   else
    magValue = (unsigned long) rint(doubleValue);
  err = AECreateDesc(&magValue);
 return err;
}
