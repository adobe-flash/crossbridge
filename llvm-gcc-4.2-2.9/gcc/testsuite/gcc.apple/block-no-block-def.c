/* APPLE LOCAL radar 6230656 */
/* { dg-options "-fno-blocks" } */
/* { dg-do compile { target *-*-darwin* } } */

#if __block
#error
#endif
