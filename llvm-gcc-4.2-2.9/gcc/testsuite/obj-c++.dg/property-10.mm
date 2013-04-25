/* APPLE LOCAL file 4564386 */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

@protocol GCObject
@property int Age;
@end

@protocol DerivedGCObject <GCObject>
@property int Dclass;
@end

@interface GCObject  : Object <DerivedGCObject> {
    int ifield;
    int iOwnClass;
    int iDclass;
}
@property int OwnClass;
@end

@implementation GCObject : Object
@synthesize Age=ifield;
@synthesize Dclass=iDclass;
@synthesize OwnClass=iOwnClass;
@end

int main(int argc, char **argv) {
    GCObject *f = [GCObject new];
    f.Age = 5;
    f.Dclass = 1;
    f.OwnClass = 3;
    return f.Age + f.Dclass  + f.OwnClass - 9;
}
