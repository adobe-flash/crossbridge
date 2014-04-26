/* APPLE LOCAL file mainline */
/* Check if finding multiple signatures for a method is handled gracefully when method lookup succeeds (see also method-7.m).  */
/* Contributed by Ziemowit Laski <zlaski@apple.com>  */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@protocol MyObject
- (id)initWithData:(Object *)data;
@end

@protocol SomeOther
- (id)initWithData:(int)data;
@end

@protocol MyCoding
- (id)initWithData:(id<MyObject, MyCoding>)data;
@end

@interface NTGridDataObject: Object <MyCoding>
{
    Object<MyCoding> *_data;
}
+ (NTGridDataObject*)dataObject:(id<MyObject, MyCoding>)data;
@end

@implementation NTGridDataObject
- (id)initWithData:(id<MyObject, MyCoding>)data {
  return data;
}
+ (NTGridDataObject*)dataObject:(id<MyObject, MyCoding>)data
{
    NTGridDataObject *result = [[NTGridDataObject alloc] initWithData:data];
     /* { dg-warning "multiple methods named .\\-initWithData:. found" "" { target *-*-* } 34 } */
     /* { dg-warning "using .\\-\\(id\\)initWithData:\\(Object \\*\\)data." "" { target *-*-* } 10 } */
     /* { dg-warning "also found .\\-\\(id\\)initWithData:\\(id <MyObject, MyCoding>\\)data." "" { target *-*-* } 18 } */
     /* { dg-warning "also found .\\-\\(id\\)initWithData:\\(int\\)data." "" { target *-*-* } 14 } */

     /* The following warning is a consequence of picking the "wrong" method signature.  */
     /* APPLE LOCAL mainline */
     /* { dg-warning "incompatible Objective-C types \\'id\\', expected \\'struct Object \\*\\' when passing argument 1 of \\'initWithData:\\' from distinct" "" { target *-*-* } 34 } */
    return result;
}
@end

/* { dg-options "-Wstrict-selector-match" } */

