/* APPLE LOCAL file radar 2995215 */
/* Test for generation of utf-16 cfstring */
/* { dg-options "-std=c99 -mmacosx-version-min=10.5" } */
/* { dg-do compile { target powerpc-*-darwin* } } */

#import <Foundation/Foundation.h>

int main(void) {
	NSLog(@"\u2192 \u2603 \u2190");
	NSLog(@"\U00010400\U0001D12B");
	return 0;
}
/* { dg-final { scan-assembler ".section __TEXT,__ustring" } } */
/* { dg-final { scan-assembler "___utf16_string_1:\n\t.byte\t33\n\t.byte\t-110\n\t.byte\t0\n\t.byte\t32" } } */
/* { dg-final { scan-assembler "___utf16_string_2:\n\t.byte\t-40\n\t.byte\t1\n\t.byte\t-36\n\t.byte\t0" } } */
