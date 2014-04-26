/* APPLE LOCAL file 5795493 - blocks */
/* Check that function decl and definition which have blocks (blocks) as their
   type do the type-matching correctly and do not issue bogus warning. */
/* { dg-do compile } */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC -fblocks" { target *-*-darwin* } } */

void useBlock (void (^arg)(void));

void useBlock (void (^arg)(void)) {}

@interface Fred
- (void) useBlock:(void (^)(void)) aBlock;
@end

@implementation Fred
- (void) useBlock:(void (^)(void)) aBlock { }
@end

