/* APPLE LOCAL file CW asm blocks */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options { -fasm-blocks } } */
/* Radar 4766972 */

int f() { @@ }	/* { dg-error "expected unqualified-id before '@' token" } */
		/* { dg-error "expected `;' before '@' token" "" { target *-*-* } 6 } */
