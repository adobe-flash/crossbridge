/* APPLE LOCAL file radar 4156731 */
/* { dg-do compile } */

@interface A

- (void)test; 

@end

extern int foo();

void baz()
{
    [foo test];	/* { dg-warning "invalid receiver type" } */
}
