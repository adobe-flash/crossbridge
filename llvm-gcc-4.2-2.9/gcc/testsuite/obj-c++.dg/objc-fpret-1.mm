/* APPLE LOCAL begin radar 4280641 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* APPLE LOCAL 64  bit default */
/* { dg-require-effective-target ilp32 } */
#include <objc/Object.h>

@interface Example : Object
        float FLOAT;
        double DOUBLE;
	long double LONGDOUBLE;
        id    ID;
@end

@implementation Example
 - (double) RET_DOUBLE
   {
	return DOUBLE;
   }
 - (float) RET_FLOAT
   {
	return FLOAT;
   }
 - (long double) RET_LONGDOUBLE
   {
	return LONGDOUBLE;
   }
@end

int main()
{
	Example* pe;
	double dd = [pe RET_DOUBLE];
	dd = [pe RET_FLOAT];
	dd = [pe RET_LONGDOUBLE];
}

/* { dg-final { scan-assembler "objc_msgSend_fpret" } } */

/* { dg-final { scan-assembler-not "objc_msgSend\[^_S\]" } } */
/* APPLE LOCAL end radar 4280641 */
