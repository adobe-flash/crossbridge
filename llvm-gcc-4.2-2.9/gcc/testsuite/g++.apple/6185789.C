/* APPLE LOCAL file 6185789 */
/* { dg-do run } */
/* { dg-options { "-O2" } } */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
extern "C" {
#pragma pack(push, 4)
  typedef int64_t FigTimeValue;
  typedef int32_t FigTimeScale;
  typedef int64_t FigTimeEpoch;
  enum {
    kFigTimeFlags_Valid = 1UL<<0,
    kFigTimeFlags_HasBeenRounded = 1UL<<1,
    kFigTimeFlags_PositiveInfinity = 1UL<<2,
    kFigTimeFlags_NegativeInfinity = 1UL<<3,
    kFigTimeFlags_Indefinite = 1UL<<4,
    kFigTimeFlags_ImpliedValueFlagsMask = kFigTimeFlags_PositiveInfinity | kFigTimeFlags_NegativeInfinity | kFigTimeFlags_Indefinite
  };
  typedef uint32_t FigTimeFlags;
  typedef struct
  {
    FigTimeValue	value;
    FigTimeScale	timescale;
    FigTimeEpoch	epoch;
    FigTimeFlags	flags;
  } FigTime;
#pragma pack(pop)
}
FigTime
FigTimeMake(int64_t value, int32_t timescale)
{
  FigTime	tm = {value, timescale, 0, kFigTimeFlags_Valid};
  return tm;
}
const FigTime   FOO = FigTimeMake(3, 1);
int main()
{
  FigTime t1 = FOO;
  if (t1.value != FOO.value)
    abort ();
  else
    return 0;
}
