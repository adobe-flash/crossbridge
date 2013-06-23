#include <Foundation/Foundation.h>
#include <stdlib.h>
#include <stdio.h>

#define D() printf("exceptions.m:%d\n", __LINE__)

@interface A @end
@implementation A @end

void f0() {
  @throw(@"hi");
}

int f1(int return_in_finally) {
  @try {
    D();
    f0();
    D();
    return 0;
  } @finally {
    D();
    if (return_in_finally)
      return 1;
  }
  return 2;
}

int f2(int return_in_catch) {
  @try {
    D();
    f0();
    D();
    return 0;
  } @catch(A *x) {
  } @catch(...) {
    D();
    if (return_in_catch)
      return 1;
  }
  return 2;
}

int f3() {
  @try {
    D();
    f0();
    D();
    return 0;
  } @catch(...) {
    D();
    @throw(@"hello");
    D();
  }
  return 2;
}

void t1() {
  @try {
    D();  
    printf("f1() = %d\n", f1(1));
    D();
    printf("f1() = %d\n", f1(0));
    D();
  } @catch(id e) {
    D();
  } @finally {
    D();
  }
  D();
}

void t2() {
  @try {
    D();  
    printf("f2() = %d\n", f2(1));
    D();
    printf("f2() = %d\n", f2(0));
    D();
  } @catch(id e) {
    D();
  } @finally {
    D();
  }
  D();
}

void t3() {
  @try {
    D();  
    printf("f3() = %d\n", f3());
    D();
  } @catch(id e) {
    D();
  } @finally {
    D();
  }
  D();
}

void t4() {
 int i;

 @try {
   for (i=0; i<10; ++i) {
     @try {
       printf("i=%d\n", i);
       // These all end up being a continue due to the finally block.
       if (i==2) return;
       D();
       if (i==3) @throw(@"x");
       D();
       if (i==4) break;
       D();
       if (i==5) @throw([[NSObject alloc] init]);
       D();
       if (i==6) continue;
       D();
     } @catch (NSString *e) {
       D();
       @throw;       
     } @catch (id e) {
       D();
       @throw;
     } @finally {
       D();
       if (i==8)
         return;
       D();
       continue;
     }
   }
 } @finally {
   D();
 }
}

void t5() {
 @try {
   return;
 } @finally {
   D();
 }
}

int main() {
  id Pool = [[NSAutoreleasePool alloc] init];

  t1();
  t2();
  t3();

  // Disabled for now until we move to USE_REFERENCE_OUTPUT, gcc/llvm-gcc get
  // the semantics wrong on x86_64.
#if 0
  t4();
#endif

  t5();

  [Pool release];

  return 0;
}
