/* APPLE LOCAL file radar 6936421 */
/* Test that no seg. fault is generated due to initializer being a property
   getter of a class with copy constructor. */
/* { dg-options "-mmacosx-version-min=10.5" { target *-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

struct vector {
  vector();
  vector(const vector &);
};

typedef vector operations_t;


@interface GridManager 
{
 operations_t operations;
}

@property (readonly) operations_t operations;
@end

@interface MainView {
 GridManager *gmgr;
}
- (void)placeOperations;
@end

void cvt(operations_t);

@implementation MainView
-(void)placeOperations
{
  operations_t operations = gmgr.operations;
  operations = gmgr.operations;

  (operations_t)gmgr.operations;
  cvt(gmgr.operations);
}

@end
