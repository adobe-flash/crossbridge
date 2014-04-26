/* APPLE LOCAL file radar 2995215 */
/* Test for generation of utf-16 cfstring */
/* { dg-options "-std=c99 -mmacosx-version-min=10.5" } */
/* { dg-do compile { target i?86-*-darwin* } } */

#import <Foundation/Foundation.h>

int main(void) {
	NSLog(@"\u2192 \u2603 \u2190");
	NSLog(@"\U00010400\U0001D12B");
	return 0;
}
/* LLVM LOCAL accept llvm syntax */
/* { dg-final { scan-assembler ".section.*__TEXT,__ustring" } } */
/* LLVM LOCAL begin same data, very different syntax */
/* { dg-final { scan-assembler "___utf16_string_1:.*\n\t(.byte\t-110\n\t.byte\t33\n\t.byte\t32\n\t.byte\t0|.asciz\t\"\\\\622! \\\\000|.asciz\t\"\\\\222! \\\\000)" } } */
/* { dg-final { scan-assembler "___utf16_string_2:.*\n\t(.byte\t1\n\t.byte\t-40\n\t.byte\t0\n\t.byte\t-36|.asciz\t\"\\\\001\\\\730\\\\000\\\\734)" } } */
/* LLVM LOCAL end */

