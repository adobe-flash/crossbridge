/* APPLE LOCAL file radar 4506903 */
/* Test for property lookup in a protocol id. */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@protocol NSCollection
@property(readonly) int  count;
@end

static int testCollection(id <NSCollection> collection) {
    return collection.count;
}
