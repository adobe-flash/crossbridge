/* APPLE LOCAL file AltiVec 5527030 */
/* { dg-do compile } */
/* { dg-final { scan-assembler-not "_GLOBAL__I_" } } */

typedef float vFloat __attribute__ ((__vector_size__ (16)));
static const vFloat _minusZero = (const vFloat) { -0.0f, -0.0f, -0.0f, -0.0f };
const vFloat *p = &_minusZero;
