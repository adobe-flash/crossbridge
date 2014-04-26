/* APPLE LOCAL file radar 6261630 */
/* Test use of trailing attribute in a method return type. */
/* { dg-options "-fobjc-gc" } */
/* { dg-do compile { target *-*-darwin* } } */

@interface INTF
- (const char * __attribute__((objc_gc(strong))))encoding;
- (id  __strong)encoding1;
- (const char * __weak)encoding2;
- (const char ** __strong)encoding3;
@end

