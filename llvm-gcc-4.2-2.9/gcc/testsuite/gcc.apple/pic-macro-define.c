/* APPLE LOCAL begin radar 4224728 */
/* { dg-do run { target "i?86-*-*" powerpc*-*-* } } */
/* { dg-options "-fPIC" } */

#if defined __PIC__ 
int main() {
	return 0;
}
#else
  error "NO __PIC__ DEFINED"	
#endif
/* APPLE LOCAL end radar 4224728 */
