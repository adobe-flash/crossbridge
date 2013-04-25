/* APPLE LOCAL file ObjC GC */
/* Using -fobjc-gc should not mess with how 'super' gets handled. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */
/* Contributed by Ziemowit Laski <zlaski@apple.com>  */

#include <objc/Object.h>

typedef struct _GRDataPoint *GRDataPoint;
typedef struct _NSRange {
  unsigned int location;
  unsigned int length;
} NSRange;

typedef struct {
  id baz;
} SmallStruct;
typedef struct {
  SmallStruct bar;
} InnerStruct;

typedef struct {
  InnerStruct foo;
} ComplexStruct;

@interface SomeOther : Object
- (void)setSelectedRange:(NSRange)charRange;
@end

@interface GRDataSet : Object {
@protected
  int numElements;
  GRDataPoint * dataPoints;
@private
  id _dataSource;
}
-(BOOL) setSelectedRange:(NSRange)r;
-(void) dealloc;
@end

@interface GRPieDataSet : GRDataSet {
@private
   double _sum;
}
@end

@implementation GRPieDataSet
-(void) dealloc
{
  dataPoints = 0;
  [super dealloc];
}
-(BOOL) setSelectedRange:(NSRange)r
{
  BOOL retval;
  retval = [super setSelectedRange:r];
  return retval;
}
-(void) modStruct:(ComplexStruct)s
{
  s.foo.bar.baz = nil;
}
@end

/* { dg-final { scan-assembler "objc_msgSendSuper" } } */
/* { dg-final { scan-assembler-not "objc_msgSend\[^S\]" } } */
/* { dg-final { scan-assembler-not "objc_assign_strongCast" } } */
