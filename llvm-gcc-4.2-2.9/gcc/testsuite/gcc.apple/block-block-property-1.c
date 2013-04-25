/* APPLE LOCAL file radar 5831920 */
/* Test a property with block type. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC" { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Foundation/Foundation.h>
#include <stdio.h>

void * _NSConcreteStackBlock[32];

@interface TestObject {

}
@property(copy, readonly) int (^getIntCopy)(void);
@property(retain, readonly) int (^getIntRetain)(void);
@end



int main(char *argc, char *argv[]) {
    return 0;
}
