/* APPLE LOCAL file 4174833 */
/* Check that the STABS information for ObjC objects is flattened;
   gdb does not (yet) know how to traverse ObjC class hierarchies.  */
/* Contributed by Ziemowit Laski <zlaski@apple.com> */

/* { dg-do compile } */
/* { dg-skip-if "No stabs" { mmix-*-* *-*-aix* alpha*-*-* hppa*64*-*-* ia64-*-* } { "*" } { "" } } */
/* { dg-options "-gstabs" } */
/* LLVM LOCAL llvm doesn't currently support stabs. */
/* { dg-require-stabs "" } */

#include <objc/Object.h>

@interface Base: Object {
  int a;
  id b;
}
- meth;
@end

@interface Derived: Base {
  Object *c;
  Base *d;
}
- meth;
@end

@implementation Base
- meth { a = 0; return self; }
@end

@implementation Derived
- meth { d = self; return self; }
@end

/* { dg-final { scan-assembler "Base:T\\(\[0-9\]+,\[0-9\]+\\)=s\[0-9\]+isa:\\(\[0-9\]+,\[0-9\]+\\),0,\[0-9\]+" } } */
/* { dg-final { scan-assembler "Derived:T\\(\[0-9\]+,\[0-9\]+\\)=s\[0-9\]+isa:\\(\[0-9\]+,\[0-9\]+\\),0,\[0-9\]+;a:\\(\[0-9\]+,\[0-9\]+\\)" } } */

