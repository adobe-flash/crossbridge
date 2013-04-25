/* APPLE LOCAL file 6310728 */
/* { dg-do compile { target "i?86-*-*" } } */
/* { dg-options { -mfix-and-continue -m64 } } */
typedef int int32_t;
typedef int32_t SINT32 ;
void RowCol8x8( SINT32 *in8x8,    SINT32 *out8x8,    SINT32 acqfact,    SINT32 intraFlag,    float *i2f9808div2,    float *i2f9239div2,    float *i2f7071div2,    char *qAC) {
  SINT32 k;
  float tmpout[64];
  SINT32 *ptrL;
  float *ptrF;
  SINT32 tmpL0,tmpL1,tmpL2,tmpL3,tmpL4,tmpL5,tmpL6,tmpL7,tmpL8,tmpL9;
  float kess7071div2 = 0.3535533905932737;
  float kess9808div2 = 0.4903926402016152;
  float kess8315div2 = 0.4157348061512726;
  float kess5556div2 = 0.2777851165098011;
  float kess1951div2 = 0.09754516100806417;
  float kess9239div2 = 0.4619397662556434;
  float kess3827div2 = 0.1913417161825449;
  float tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8,tmp9;
  for (ptrL=in8x8,ptrF=tmpout,k=0;
       k<8;
       k++,ptrF++,ptrL++)  {
    tmp1 = ptrF[0];
    tmp2 = ptrF[56];
    tmp4 = ptrF[48];
    if (intraFlag && k == 0)   {
      tmp5 = 0.5 + tmp5*0.125;
    }
    tmpL1 = tmp5;
    tmpL2 = tmp7 - kess7071div2*tmp8;
    tmp7 = kess9239div2*tmp0;
    tmpL4 = tmp7 + kess3827div2*tmp1;
    tmp8 = kess3827div2*tmp0;
    tmpL7 = tmp8 - kess9239div2*tmp1;
    tmp5 = kess9808div2*tmp9;
    tmp5 += kess8315div2*tmp2;
    tmp5 += kess5556div2*tmp4;
    tmpL3 = tmp5 + kess1951div2*tmp6;
    tmp8 += kess8315div2*tmp4;
    tmpL8 = tmp8 - kess9808div2*tmp6;
    if (!(intraFlag && k == 0))   {
      if (!( acqfact <= 7 ))    {
        ptrL[0] = (SINT32) qAC[(tmpL1)];
      }
      else     ptrL[0] = tmpL1;
    }
    if (!( acqfact <= 7 ))   {
      ptrL[16] = (SINT32) qAC[(tmpL4)];
      ptrL[24] = (SINT32) qAC[(tmpL5)];
      ptrL[40] = (SINT32) qAC[(tmpL6)];
      ptrL[48] = (SINT32) qAC[(tmpL7)];
      ptrL[56] = (SINT32) qAC[(tmpL8)];
    }
    else   {
      ptrL[32] = tmpL2;
      ptrL[8] = tmpL3;
      ptrL[16] = tmpL4;
      ptrL[48] = tmpL7;
      ptrL[56] = tmpL8;
    }
  }
}
