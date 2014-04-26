/* APPLE LOCAL file radar 4477797 */
/* { dg-options "-gdwarf-2 -dA" } */
/* LLVM LOCAL begin allow for asciz instead of ascii */
/* { dg-final { scan-assembler "\"Object(\\\\0)?\".*DW_AT_name" } } */
/* { dg-final { scan-assembler "\"MyObject(\\\\0)?\".*DW_AT_name.*DW_TAG_inheritance" } } */
/* LLVM LOCAL end */
#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@interface MyObject : Object {
  int myData;
}
@end

@implementation MyObject @end

int main()
{
  MyObject *obj = [[MyObject alloc] init];
}	
