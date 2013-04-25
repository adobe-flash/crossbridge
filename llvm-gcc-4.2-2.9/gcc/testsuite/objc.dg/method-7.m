/* APPLE LOCAL file mainline */
/* Check if finding multiple signatures for a method is handled gracefully.  Author:  Ziemowit Laski <zlaski@apple.com>  */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface Class1
- (void)setWindow:(Object *)wdw;
@end

@interface Class2
- (void)setWindow:(Class1 *)window;
@end

id foo(void) {
  Object *obj = [[Object alloc] init];
  id obj2 = obj;
  [obj setWindow:nil];  /* { dg-warning ".Object. may not respond to .\\-setWindow:." } */
       /* { dg-warning "Messages without a matching method signature" "" { target *-*-* } 19 } */
       /* { dg-warning "will be assumed to return .id. and accept" "" { target *-*-* } 19 } */
       /* { dg-warning ".\.\.\.. as arguments" "" { target *-*-* } 19 } */
  [obj2 setWindow:nil]; /* { dg-warning "multiple methods named .\\-setWindow:. found" } */
       /* { dg-warning "using .\\-\\(void\\)setWindow:\\(Object \\*\\)wdw." "" { target *-*-* } 9 } */
       /* { dg-warning "also found .\\-\\(void\\)setWindow:\\(Class1 \\*\\)window." "" { target *-*-* } 13 } */

  return obj;
}

/* { dg-options "-Wstrict-selector-match" } */
