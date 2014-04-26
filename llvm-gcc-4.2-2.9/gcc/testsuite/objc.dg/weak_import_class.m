/* APPLE LOCAL file radar 7865106 */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.6 -fobjc-abi-version=2" { target i?86*-*-darwin* } } */

__attribute__((weak_import)) @interface WeakRootClass @end
@implementation WeakRootClass @end


__attribute__((weak_import)) @interface WeakClass : WeakRootClass @end
@implementation WeakClass @end

@interface MySubclass : WeakClass @end
@implementation MySubclass @end

__attribute__((weak_import)) @interface CategoryImplementsThis @end
@implementation CategoryImplementsThis(MyCategory) @end
@implementation CategoryImplementsThis(YourCategory) @end

@interface Super : WeakRootClass @end
@implementation Super @end

@interface Sub : Super @end
@implementation Sub @end
/* { dg-final { scan-assembler ".weak_reference _OBJC_METACLASS_\\\$_WeakClass" } } */
/* { dg-final { scan-assembler ".weak_reference _OBJC_METACLASS_\\\$_WeakRootClass" } } */
/* { dg-final { scan-assembler ".weak_reference _OBJC_CLASS_\\\$_WeakRootClass" } } */
/* { dg-final { scan-assembler ".weak_reference _OBJC_METACLASS_\\\$_WeakRootClass" } } */
/* { dg-final { scan-assembler ".weak_reference _OBJC_CLASS_\\\$_CategoryImplementsThis" } } */
/* { dg-final { scan-assembler ".weak_reference _OBJC_METACLASS_\\\$_CategoryImplementsThis" } } */
