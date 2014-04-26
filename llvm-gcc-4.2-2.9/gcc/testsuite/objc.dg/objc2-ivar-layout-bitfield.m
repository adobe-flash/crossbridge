/* APPLE LOCAL file radar 5781140 */
/* Check for correct outputing of ivar layout when last fields are bitfields. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#import <Foundation/Foundation.h>
#import <objc/runtime.h>

@interface Foo : NSObject {
    id ivar;

    uintptr_t bitfield  :31;
    uintptr_t bitfield2 :1;
#if __LP64__
    uintptr_t bitfield3 :32;
#endif
}
@end

@implementation Foo @end

int main()
{
    if (!objc_collecting_enabled()) {
	abort ();
    }
    size_t size = class_getInstanceSize([Foo class]);
    if (size != 3*sizeof(void*)) {
	abort ();
    }
    const char *layout = class_getIvarLayout([Foo class]);
    // Desired layout: skip 0, scan 2 (isa, ivar); skip 1 (bitfield), scan 0.
    if (!layout  ||  0 != strcmp(layout, "\x02\x10")) {
	abort ();
    }
    return 0;
}

