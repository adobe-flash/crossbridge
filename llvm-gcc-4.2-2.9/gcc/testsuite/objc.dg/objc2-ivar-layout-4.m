/* APPLE LOCAL file radar 5251019 */
/* Test that we add the number of bytes to skip at tail end of ivar layout. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */


#include <objc/runtime.h>
#include <Foundation/Foundation.h>

@interface X_NSKeyValueObservance : NSObject {
    NSUInteger _retainCount;
    __weak NSObject *_observer;
    NSObject *_property;
    NSKeyValueObservingOptions _options;
    void *_context;
    void * _cachedIsShareable;
}
@end

@implementation X_NSKeyValueObservance
@end

const char expected_strong [] = {0x01, 0x21, 0x30, 0};
const char expected_weak [] = {0x21, 0x40, 0x00};

int main()
{
        int i = 0;
        const char *ivar_layout_string = (const char*)class_getIvarLayout([X_NSKeyValueObservance class]);
        if (strcmp (ivar_layout_string, expected_strong))
          abort ();

        ivar_layout_string = (const char*)class_getWeakIvarLayout([X_NSKeyValueObservance class]);
        if (strcmp (ivar_layout_string, expected_weak))
          abort ();
        return 0;
}

