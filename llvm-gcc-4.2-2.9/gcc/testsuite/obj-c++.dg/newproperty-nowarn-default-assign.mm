/* APPLE LOCAL file radar 5172645 */
/* Don't issue 'assign' is default warning with -Wno-property-assign-default */
/* { dg-options "-mmacosx-version-min=10.5 -Wno-property-assign-default -fobjc-gc" } */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-require-effective-target objc_gc } */
#include <Foundation/Foundation.h>

@interface Foo : NSObject
  @property id bar;
@end

@implementation Foo
  @dynamic bar;
@end

