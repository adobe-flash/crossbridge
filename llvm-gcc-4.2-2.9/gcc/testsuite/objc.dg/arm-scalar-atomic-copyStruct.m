/* APPLE LOCAL file 6671703 use objc_copyStruct() for big scalars */
/* { dg-do compile { target arm*-*-darwin* } } */
/* { dg-options "-O0" } */

#import <Foundation/NSObject.h>


@interface myClass : NSObject {
  unsigned long long _i64;
}

@property (readwrite) unsigned long long i64;

@end

@implementation myClass
@synthesize i64 = _i64;



@end

extern int printf(const char *fmt, ...);

void foo(myClass *obj)
{
  printf ("%llx\n", obj.i64);
}

int main()
{
  myClass *obj = [[myClass alloc] init];
  obj.i64 = 0xfeedface55555555LL;
  foo (obj);
  return 0;
}

/* { dg-final { scan-assembler-times "blx\t_objc_copyStruct" 2 } } */
