/* { dg-do compile } */

/* Another gimplifier ICE... */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface MyView: Object {
  int _frame;
}
- (void)_finalize;
@end

@interface MyViewTemplate: MyView {
  void *_className;
}
- (id)createRealObject;
@end

@implementation MyViewTemplate
- (id)createRealObject {
    id realObj;
    *(MyView *)realObj = *(MyView *)self;
    return realObj;
}
@end
