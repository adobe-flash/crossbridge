/* APPLE LOCAL file CW asm blocks */
/* { dg-do assemble { target powerpc*-*-* } } */
/* { dg-options "-fasm-blocks" } */
/* Radar 4258924 */

struct AsmAlignCodeSample {
  void AsmAlignCode();
  static void NextLabelFunction();
};

asm void AsmAlignCodeSample::AsmAlignCode() {
  b NextLabelFunction
  .align 4
}
