/* APPLE LOCAL file radar 5925639 */
/* { dg-options "-std=c99 -mmacosx-version-min=10.5" } */
/* { dg-do compile } */

static int test_NSURLGetResourceValueForKey( id keys )
{
 for ( id key; in keys) { /* { dg-error "expected expression before" } */
  } 
}
