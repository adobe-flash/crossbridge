/* APPLE LOCAL file radar 5265737 */
/* Test that aggregate-valued properties work as expected. */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-framework Foundation" { target arm*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */
#include <Foundation/Foundation.h>

extern "C" void abort (void);

static CGRect return_val = { {1.0, 2.0}, {3.0, 4.0}};

CGRect MyCGRectMake(CGFloat x, CGFloat y, CGFloat width, CGFloat height)
{
  return return_val;
}


@interface CALayer : NSObject
{
  CGRect _bounds;
}

@property CGRect bounds;
@end

@implementation CALayer

@synthesize bounds = _bounds;

@end

int main(int argc, char *argv[])
{

 CGRect bounds;
 CALayer *layer = [CALayer new];
 layer.bounds = MyCGRectMake(0, 0, 200, 200);

 if (layer.bounds.origin.x != 1.0 || layer.bounds.size.height != 4.0)
   abort ();

 CALayer *naked_layer = [CALayer new];
 naked_layer.bounds = return_val;

 if (naked_layer.bounds.origin.x != layer.bounds.origin.x || naked_layer.bounds.size.width != layer.bounds.size.width)
   abort ();

  return 0;
}

