/* APPLE LOCAL file radar 5831920  - modified for radar 6255671 */
/* Test a property with block type. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC -framework Foundation" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>

void * _NSConcreteStackBlock[32];
@interface TestObject : NSObject {

int (^getIntCopy)(void);

int (^getIntRetain)(void);

}
@property (assign) int (^getIntCopy)(void);
@property (assign) int (^getIntRetain)(void);
@end

@implementation TestObject
@synthesize getIntCopy;
@synthesize getIntRetain;

@end

int DoBlock (int (^getIntCopy)(void))
{
  return getIntCopy();
}



int main(char *argc, char *argv[]) {
    int count;
    __block int val = 0;
    TestObject *to = [[TestObject alloc] init];
    to.getIntRetain = ^ { printf("\n Hello(%d)\n", val); return ++val; }; 
    to.getIntCopy = to.getIntRetain;

    count = DoBlock (to.getIntCopy);
    if (count != 1)
      abort();
    count = DoBlock (to.getIntRetain);
    count = DoBlock (to.getIntRetain);
    return count - 3;
}
