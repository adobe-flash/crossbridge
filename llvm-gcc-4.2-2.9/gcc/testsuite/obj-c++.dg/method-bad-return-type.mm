/* APPLE LOCAL file radar 5424416 */
/* Methods cannot return an array. */

int v[2];

@interface Foo
-(int[2])x;
@end

@implementation Foo
-(int[2])x		/* { dg-error "\'x\' declared as method returning an array" } */
{
        return v;	/* { dg-error "cannot convert" } */
}
@end

