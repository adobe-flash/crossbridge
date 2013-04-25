/* APPLE LOCAL file mainline 2006-04-19 4314956 */
/* { dg-do compile { target i?86*-*-* } }*/
/* { dg-options "-O3" } */
/* { dg-final { scan-assembler "\tjmp\t.*memset*" } } */
typedef __SIZE_TYPE__ size_t;
extern "C" void memset(void *, int, size_t);
class SIMDProcessor {
  public:
    virtual void Memset(void *, const int, const int) = 0;
};

class SIMDGeneric : public SIMDProcessor {
  public:
    virtual void Memset(void *, const int, const int);
};

void SIMDGeneric::Memset(void *dst, const int val, const int count) {
  memset(dst, val, count);
}
