/* APPLE LOCAL file 5465109 - modified for radar 6255913 */
/*
The following test program should emit metadata for _OBJC_PROTOCOL_$_Proto2 and _OBJC_PROTOCOL_$__Proto1. 
 This root emits both protocols and puts them both in __protocol_list.
*/
// should emit Proto2 only. Proto1 should not be emitted.
/* { dg-options "-mmacosx-version-min=10.5 -fobjc-abi-version=2" { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */


@protocol Proto1
+method;
@end

@protocol Proto2  <Proto1>
+method2;
@end

long foo ()
{
	return (long)@protocol(Proto2);
}
/* { dg-final { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto1:" } } */
/* { dg-final { scan-assembler "l_OBJC_PROTOCOL_\\\$_Proto2:" } } */
