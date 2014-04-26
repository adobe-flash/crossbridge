/* APPLE LOCAL file radar 4577849 */
/* Test for encoding of type strings for instance variables and 
   properties of type id <P1, ..., Pn> */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@protocol X, Y, Z;
@class Foo;

@protocol Proto
@property (copy) id <X> x;
@property (copy) id <X, Y> xy;
@property (copy) id <X, Y, Z> xyz;
@property(copy)  Foo <X, Y, Z> *fooxyz;
@end

@interface Intf <Proto>
{
id <X> IVAR_x;
id <X, Y> IVAR_xy;
id <X, Y, Z> IVAR_xyz;
Foo <X, Y, Z> *IVAR_Fooxyz;
}
@end

@implementation Intf 
@dynamic x, xy, xyz, fooxyz;
@end

/* LLVM LOCAL begin accept llvm syntax (asciz) */
/* { dg-final { scan-assembler ".asci.*T\@.*<X><Y><Z>.*" } } */
/* { dg-final { scan-assembler ".asci.*T\@.*Foo<X><Y><Z>.*" } } */
/* LLVM LOCAL end */

/**
This protocol should generate the following metadata:

struct objc_property_list __Protocol_Test_metadata = {
  sizeof(struct objc_property), 4,
  {
    { "x", "T@\"<X>\",&" },
    { "xy", "T@\"<X><Y>\",&" },
    { "xyz", "T@\"<X><Y><Z>\",&" },
    { "fooxyz", "T@\"Foo<X><Y><Z>\",&" }
  }
};

"T@\"<X><Y><Z>\",D
*/
