/* APPLE LOCAL file radar 4995066 */
/* Must compile with -m64 and -m32. */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>

static CFMutableDictionaryRef classToZombie = NULL;

void _PFZombifyObject(id object) {
    Class       cls = object->isa;
    Class       zclass = (classToZombie) ? (Class)CFDictionaryGetValue(classToZombie, cls) : NULL;
    if (!zclass) {
        if (!classToZombie) {
            classToZombie = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
        }
        id tmp = NSAllocateObject(cls, 0, nil);
        NSDeallocateObject(tmp);
        zclass = *(Class*)tmp;
        CFDictionarySetValue(classToZombie, cls, zclass);
    }
    object->isa = zclass;  // line 18
}

