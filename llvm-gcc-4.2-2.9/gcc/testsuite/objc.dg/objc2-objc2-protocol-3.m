/* APPLE LOCAL file radar 6351990 */
/* { dg-options "-mmacosx-version-min=10.6 -m32 -fobjc-abi-version=2" { target *-*-darwin* } } */
/* { dg-do compile { target *-*-darwin* } } */

@protocol PROTO
@end
@protocol PROTO1
@end
@protocol PROTO2
@end

@interface INTF <PROTO, PROTO1, PROTO2> @end

@implementation INTF @end

int main()
{
	return (long) @protocol(PROTO); + (long) @protocol(PROTO1);
}
/* { dg-final { scan-assembler "l_OBJC_LABEL_PROTOCOL_\\\$_PROTO:\n\t.long\tl_OBJC_PROTOCOL_\\\$_PROTO" } } */
/* { dg-final { scan-assembler "l_OBJC_PROTOCOL_REFERENCE_\\\$_PROTO:\n\t.long\tl_OBJC_PROTOCOL_\\\$_PROTO" } } */
