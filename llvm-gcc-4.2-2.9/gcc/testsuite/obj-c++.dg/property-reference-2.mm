/* APPLE LOCAL file radar 6029624 */
/* Test for const refrence property types. */
/* { dg-options "-m64 -mmacosx-version-min=10.5" } */
/* { dg-do compile { target *-*-darwin* } } */

struct MyStruct
{
        float x;
        float y;
        float z;
};

@interface MyClass

@property (assign, readwrite) const MyStruct& foo;
@property (assign, readwrite) const MyStruct& Myfoo;

@end

@implementation MyClass

@synthesize Myfoo;
@synthesize foo;

@end
