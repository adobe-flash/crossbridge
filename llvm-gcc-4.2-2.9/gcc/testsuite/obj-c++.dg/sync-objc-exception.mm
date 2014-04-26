/* APPLE LOCAL file radar 5982990 */
/* This tests that local variables declared in @synchronized block
   have are make volatile. Test should compile with no warning or
   error.
*/
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-exceptions -Os -Wextra -Werror" } */
/* { dg-require-effective-target ilp32 } */
 
struct MyPoint { int x; };

@interface MyObject {}
+(void) doNothingWithPoint:(struct MyPoint)aPoint;
@end

int main()
{
    id  pool = 0;
	
    @synchronized(pool)
	{
		struct MyPoint thePoint;
		[MyObject doNothingWithPoint:thePoint];
	}
	
    return 0;
}
