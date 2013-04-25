/* APPLE LOCAL file radar 4805321 */
/* Test for property lookup in a protocol id. */
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-new-property" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fobjc-new-property" { target arm*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@protocol NSCollection
@property(assign, readonly) int  count;
@end

static int testCollection(id <NSCollection> collection) {
    return collection.count;
}
