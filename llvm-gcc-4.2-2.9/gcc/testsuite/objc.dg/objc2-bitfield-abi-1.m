/* APPLE LOCAL file radar 5025001 */
/* Test that gcc apppends a char :0; to each class with ivars which has a bitfield as its last. */
/* { dg-options "-mmacosx-version-min=10.5 -m64 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do run { target *-*-darwin* } } */

#include <objc/Object.h>
#include <stdio.h>
extern void abort(void);

@interface Base {
    id isa;
}
@end
@implementation Base @end

@interface Super : Base {
    char b1 :1;
    char b2 :1;
    char b3 :1;
    char b4 :1;
    char b5 :1;
    char b6 :1;
    char :0;
}
@end
@implementation Super @end

@interface Sub : Super {
    char b7 :1;
    char b8 :1;
    char :0;
}
@end
@implementation Sub @end


@interface Base2 {
    id isa;
}
@end
@implementation Base2 @end

@interface Super2 : Base2 {
    char b1 :1;
    char b2 :1;
    char b3 :1;
    char b4 :1;
    char b5 :1;
    char b6 :1;
}
@end
@implementation Super2 @end

@interface Sub2 : Super2 {
    char b7 :1;
    char b8 :1;
}
@end
@implementation Sub2 @end

int main()
{
    // definitions with and without `char :0;` should behave the same

    if(class_getInstanceSize(objc_getClass("Base")) !=
           class_getInstanceSize(objc_getClass("Base2")))
	abort ();
    if(class_getInstanceSize(objc_getClass("Super")) !=
           class_getInstanceSize(objc_getClass("Super2")))
	abort ();
    if(class_getInstanceSize(objc_getClass("Sub")) !=
           class_getInstanceSize(objc_getClass("Sub2")))
	abort ();

    return 0;
}
