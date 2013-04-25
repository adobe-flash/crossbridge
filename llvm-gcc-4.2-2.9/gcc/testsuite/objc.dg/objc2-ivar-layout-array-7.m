/* APPLE LOCAL file radar 5082000 */
/* Test of ivar layour involving array ivars. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#include <objc/runtime.h>
#include <Foundation/Foundation.h>

typedef struct _NSAttributeDictionaryElement {
    NSUInteger hash;
    id key;
    id value;
} NSAttributeDictionaryElement;

@interface NSTableOptions : NSObject {
    NSUInteger numElements;
    __weak id refCount;
    NSAttributeDictionaryElement elements[3];
    __weak id tail_end;
}
@end

@implementation NSTableOptions
@end

const char expected_strong [] = {0x01, 0x32, 0x12, 0x12, 0x10, 0};

const char expected_weak [] = {0x21, 0x91, 0};

int main()
{
        int i = 0;
        const char *ivar_layout_string = (const char*)class_getIvarLayout([NSTableOptions class]);
        if (strcmp (ivar_layout_string, expected_strong))
          abort ();

	ivar_layout_string = (const char*)class_getWeakIvarLayout([NSTableOptions class]);
	if (strcmp (ivar_layout_string, expected_weak))
	  abort ();

        return 0;
}

