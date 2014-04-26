/* APPLE LOCAL begin radar 4122328 */
/* { dg-do compile { target i?86-*-darwin*i powerpc*-*-darwin* } } */
/* { dg-options "-fasm-blocks" } */
@class NSString; /* { dg-error "expected identifier or '\\(' before '@' token" } */

int main()
{
}
/* APPLE LOCAL end radar 4122328 */
