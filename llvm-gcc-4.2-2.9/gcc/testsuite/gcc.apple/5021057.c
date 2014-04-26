/* APPLE LOCAL file 5021057 */
/* { dg-do compile { target "powerpc*-*-darwin*" } } */
/* { dg-options "-faltivec" } */
void foo () {
  __attribute__ ((altivec(vector__))) unsigned short vBits;
  vBits = vec_splat (vBits_xfer.v, 0); 
/* { dg-error "'vBits_xfer' undeclared \\(first use in this function\\)"  "" { target *-*-* } 6 } 
   { dg-error "\\(Each undeclared identifier is reported only once" "" { target *-*-* } 6 }
   { dg-error "for each function it appears in.\\)" "" { target *-*-* } 6 }
   { dg-error "invalid argument\\(s\\) for AltiVec operation or predicate|incompatible types in assignment" "" { target *-*-* } 6 }
*/
}
