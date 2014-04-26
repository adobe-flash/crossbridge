/* APPLE LOCAL file radar 4666559 */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5 -gdwarf-2 -dA" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-gdwarf-2 -dA" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */
/* LLVM LOCAL begin allow for asciz instead of ascii */
/* { dg-final { scan-assembler "\"_prop(\\\\0)?\".*DW_AT_name" } } */
@interface Foo 
{
  id isa;
  const char* _prop;
}
@property const char* prop;
@end

@implementation Foo 
@synthesize prop = _prop;
@end
