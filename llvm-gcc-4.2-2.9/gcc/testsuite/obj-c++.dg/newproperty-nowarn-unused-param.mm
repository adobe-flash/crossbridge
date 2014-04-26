/* APPLE LOCAL file radar 5232840 - radar 5398274 */
/* Test that no warning is issued on 'unused' "_value" parameter even though it is used. */
/* { dg-options "-Wunused-parameter -mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-Wunused-parameter" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
@interface MyClass 
{
@private
        id retainValue;
        id copyValue;
        id assignValue;
        id readOnlyValue;
        int foo;
}
@property(readwrite, retain) id retainValue;
@property(readwrite, assign) id assignValue;
@property(readwrite, copy) id copyValue;
@property(readonly) id readOnlyValue;

@property(readwrite) int foo;
@end


@implementation MyClass
@synthesize retainValue;
@synthesize assignValue;
@synthesize copyValue;
@synthesize readOnlyValue;

@synthesize foo;
@end
