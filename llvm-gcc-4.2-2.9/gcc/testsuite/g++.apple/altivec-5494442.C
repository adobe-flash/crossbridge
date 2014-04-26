/* APPLE LOCAL file altivec-5494442.C */
/* { dg-do compile { target powerpc*-*-* } } */
/* { dg-options "-m64 -faltivec -O2" } */
class T
{
public:
  T();
  __attribute__((altivec(vector__))) unsigned char proc1 (__attribute__((altivec(vector__))) unsigned char y);
  __attribute__((altivec(vector__))) unsigned char proc2 (__attribute__((altivec(vector__))) unsigned char uv);
};

class M
{
  void foo(unsigned ,unsigned);
public:
  M();
};

static void bar (unsigned char *savg, unsigned char *tavg,
                 unsigned char *ds, unsigned char *dt,
                 unsigned char *cur, unsigned char *cur_uv, unsigned char *out,
                 unsigned char *motin, unsigned char *m3, int even,
                 unsigned hz, unsigned h1, int l1, int f1, unsigned si1, unsigned si2, unsigned t16, T *p)
{
  int i;
  __attribute__((altivec(vector__))) unsigned char sv00, sv10, tv00, tv10, dtv00, dtv10, dsv00, dsv10;
  __attribute__((altivec(vector__))) unsigned char sv01, sv11, tv01, tv11, dtv01, dtv11, dsv01, dsv11;
  __attribute__((altivec(vector__))) unsigned char sv00e, tv00e, sv10e, tv10e, sv01e, tv01e, sv11e, tv11e;
  __attribute__((altivec(vector__))) unsigned char dsumev00, dsumev01, dsumev10, dsumev11;
  __attribute__((altivec(vector__))) unsigned char dsumov00, dsumov01, dsumov10, dsumov11;
  unsigned t16_uv = t16 >> 1;
  if (even)
    cur+=32;
  for(i = 0; i < 8; i++) {
    dsv00 = *(__attribute__((altivec(vector__))) unsigned char *)ds;
    dsv10 = *(__attribute__((altivec(vector__))) unsigned char *)(ds+16);
    dsv01 = *(__attribute__((altivec(vector__))) unsigned char *)(ds+32);
    dsv11 = *(__attribute__((altivec(vector__))) unsigned char *)(ds+48);
    dtv00 = *(__attribute__((altivec(vector__))) unsigned char *)dt;
    dtv10 = *(__attribute__((altivec(vector__))) unsigned char *)(dt+16);
    dtv01 = *(__attribute__((altivec(vector__))) unsigned char *)(dt+32);
    dtv11 = *(__attribute__((altivec(vector__))) unsigned char *)(dt+48);
    sv00 = *(__attribute__((altivec(vector__))) unsigned char *)savg;
    tv00 = *(__attribute__((altivec(vector__))) unsigned char *)tavg;
    tv10 = *(__attribute__((altivec(vector__))) unsigned char *)(tavg+16);
    sv01 = *(__attribute__((altivec(vector__))) unsigned char *)(savg+32);
    sv11 = *(__attribute__((altivec(vector__))) unsigned char *)(savg+48);
    tv01 = *(__attribute__((altivec(vector__))) unsigned char *)(tavg+32);
    tv11 = *(__attribute__((altivec(vector__))) unsigned char *)(tavg+48);
    __attribute__((altivec(vector__))) unsigned char dmtv00 = 
      *(__attribute__((altivec(vector__))) unsigned char *)motin;
    __attribute__((altivec(vector__))) unsigned char dmtv10 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+16);
    __attribute__((altivec(vector__))) unsigned char dmmv00 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+si1);
    __attribute__((altivec(vector__))) unsigned char dmmv10 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+si1+16);
    __attribute__((altivec(vector__))) unsigned char dmtv01 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+2*si1);
    __attribute__((altivec(vector__))) unsigned char dmtv11 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+2*si1+16);
    __attribute__((altivec(vector__))) unsigned char dmmv01 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+3*si1);
    __attribute__((altivec(vector__))) unsigned char dmmv11 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+3*si1+16);
    __attribute__((altivec(vector__))) unsigned char dmbv01 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+4*si1);
    __attribute__((altivec(vector__))) unsigned char dmbv11 =
      *(__attribute__((altivec(vector__))) unsigned char *)(motin+4*si1+16);
    *(__attribute__((altivec(vector__))) unsigned char *)m3 = dtv00;
    *(__attribute__((altivec(vector__))) unsigned char *)(m3+16) = dtv10;
    *(__attribute__((altivec(vector__))) unsigned char *)(m3+si1*2) = dtv01;
    *(__attribute__((altivec(vector__))) unsigned char *)(m3+si1*2+16) = dtv11;
    dtv10 = vec_max (dtv10, dmmv10);
    dtv01 = vec_max (dtv01, dmmv01);
    dtv11 = vec_max (dtv11, dmmv11);
    dtv00 = vec_max (dtv00, vec_max(dmtv00, dmtv01));
    dtv10 = vec_max (dtv10, vec_max(dmtv10, dmtv11));
    dtv01 = vec_max (dtv01, vec_max(dmtv01, dmbv01));
    dtv11 = vec_max (dtv11, vec_max(dmtv11, dmbv11));
    sv10e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (sv10,dtv10);
    sv10 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (sv10,dtv10);
    tv10e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (tv10,dsv10);
    tv10 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (tv10,dsv10);
    sv01e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (sv01,dtv01);
    sv01 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (sv01,dtv01);
    tv01e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (tv01,dsv01);
    tv01 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (tv01,dsv01);
    sv11e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (sv11,dtv11);
    sv11 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (sv11,dtv11);
    tv11e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (tv11,dsv11);
    tv11 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (tv11,dsv11);
    sv10 = (__attribute__((altivec(vector__))) unsigned char)vec_adds ( 
           (__attribute__((altivec(vector__))) unsigned short)sv10,
           (__attribute__((altivec(vector__))) unsigned short)tv10);
    sv01e = (__attribute__((altivec(vector__))) unsigned char)vec_adds (
            (__attribute__((altivec(vector__))) unsigned short)sv01e,
            (__attribute__((altivec(vector__))) unsigned short)tv01e);
    sv01 = (__attribute__((altivec(vector__))) unsigned char)vec_adds (
           (__attribute__((altivec(vector__))) unsigned short)sv01,
           (__attribute__((altivec(vector__))) unsigned short)tv01);
    sv11e = (__attribute__((altivec(vector__))) unsigned char)vec_adds (
            (__attribute__((altivec(vector__))) unsigned short)sv11e,
            (__attribute__((altivec(vector__))) unsigned short)tv11e);
    sv11 = (__attribute__((altivec(vector__))) unsigned char)vec_adds (
           (__attribute__((altivec(vector__))) unsigned short)sv11,
           (__attribute__((altivec(vector__))) unsigned short)tv11);
    tv00e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (
            (__attribute__((altivec(vector__))) unsigned short)sv00e,
            (__attribute__((altivec(vector__))) unsigned short)dsumev00);
    tv00 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (
           (__attribute__((altivec(vector__))) unsigned short)sv00e,
           (__attribute__((altivec(vector__))) unsigned short)dsumev00);
    sv11e = (__attribute__((altivec(vector__))) unsigned char)vec_mule (
            (__attribute__((altivec(vector__))) unsigned short)sv11,
            (__attribute__((altivec(vector__))) unsigned short)dsumov11);
    sv00e = *(__attribute__((altivec(vector__))) unsigned char *)(cur_uv);
    tv00e = *(__attribute__((altivec(vector__))) unsigned char *)(cur_uv+t16_uv+((f1 && i == 0) << 5));
    tv11e = *(__attribute__((altivec(vector__))) unsigned char *)(cur_uv+32+t16_uv);
    tv00 = *(__attribute__((altivec(vector__))) unsigned char *)cur;
    tv10 = *(__attribute__((altivec(vector__))) unsigned char *)(cur+t16);
    tv01 = *(__attribute__((altivec(vector__))) unsigned char *)(cur+32);
    tv11 = *(__attribute__((altivec(vector__))) unsigned char *)(cur+32+t16);
    if (even) {
      dsv00 = (__attribute__((altivec(vector__))) unsigned char)vec_mule (
              tv00e, (__attribute__((altivec(vector__))) unsigned char)(3));
      dtv00 = (__attribute__((altivec(vector__))) unsigned char)vec_mulo (
              tv00e, (__attribute__((altivec(vector__))) unsigned char )(3));
      dsv01 = (__attribute__((altivec(vector__))) unsigned char)vec_add (
              (__attribute__((altivec(vector__))) unsigned short)dsv01,
              (__attribute__((altivec(vector__))) unsigned short)dsv10);
      dtv01 = (__attribute__((altivec(vector__))) unsigned char)vec_add (
              (__attribute__((altivec(vector__))) unsigned short)dtv01,
              (__attribute__((altivec(vector__))) unsigned short)dtv10);
      dsv00 = (__attribute__((altivec(vector__))) unsigned char)vec_add (
              (__attribute__((altivec(vector__))) unsigned short)dsv00,
              (__attribute__((altivec(vector__))) unsigned short)dsv11);
      dtv00 = (__attribute__((altivec(vector__))) unsigned char)vec_add (
              (__attribute__((altivec(vector__))) unsigned short)dtv00,
              (__attribute__((altivec(vector__))) unsigned short)dtv11);
      dsv01 = (__attribute__((altivec(vector__))) unsigned char)vec_sr (
              (__attribute__((altivec(vector__))) unsigned short)dsv01,
              (__attribute__((altivec(vector__))) unsigned short)(3));
      dtv01 = (__attribute__((altivec(vector__))) unsigned char)vec_sr (
              (__attribute__((altivec(vector__))) unsigned short)dtv01,
              (__attribute__((altivec(vector__))) unsigned short)(3));
      dsv00 = (__attribute__((altivec(vector__))) unsigned char)vec_sr (
              (__attribute__((altivec(vector__))) unsigned short)dsv00,
              (__attribute__((altivec(vector__))) unsigned short)(3));
      dtv00 = (__attribute__((altivec(vector__))) unsigned char)vec_sr (
              (__attribute__((altivec(vector__))) unsigned short)dtv00,
              (__attribute__((altivec(vector__))) unsigned short)(3));
      sv01e = vec_perm (dsv01, dtv01, dsv01);
      tv01e = vec_perm (dsv00, dtv00, dsv00);
      sv00e = vec_avg (sv00e, sv01e);
      tv00e = vec_avg (tv00e, tv01e);
      sv10e = vec_avg (sv01e, sv11e);
      tv10e = vec_avg (tv01e, tv11e);
      sv10 = p->proc1 (sv10);
      tv00 = p->proc1 (tv00);
      tv10 = p->proc1 (tv10);
      sv11 = p->proc1 (sv11);
      tv01 = p->proc1 (tv01);
      tv11 = p->proc1 (tv11);
      sv00e = p->proc2 (sv00e);
      tv00e = p->proc2 (tv00e);
      sv01e = p->proc2 (sv01e);
      tv01e = p->proc2 (tv01e);
      sv10e = p->proc2 (sv10e);
      tv10e = p->proc2 (tv10e);
      sv11e = p->proc2 (sv11e);
      tv11e = p->proc2 (tv11e);
    }
    else {
      if (hz == h1) {
        *(__attribute__((altivec(vector__))) unsigned char *)(out+si2+16 ) = 
          vec_mergel (sv10e, sv00);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2 ) =
          vec_mergeh (sv01e, tv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2+16) =
          vec_mergel (sv01e, tv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2 ) =
          vec_mergeh (sv11e, sv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2+16) =
          vec_mergel (sv11e, sv01);
      }
      else
      {
        tv11e = p->proc2(tv11e);
        *(__attribute__((altivec(vector__))) unsigned char *)out =
          vec_mergeh (sv00e, tv00);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+16) =
          vec_mergel (sv00e, tv00);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+32) =
          vec_mergeh (tv00e, tv10);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+48) =
          vec_mergel (tv00e, tv10);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+si2) =
          vec_mergeh (sv10e, sv00);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+si2+16) =
          vec_mergel (sv10e, sv00);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+si2+32) =
          vec_mergeh (tv10e, sv10);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+si2+48) =
          vec_mergel (tv10e, sv10);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2) =
          vec_mergeh (sv01e, tv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2+16) =
          vec_mergel (sv01e, tv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2+32) =
          vec_mergeh (tv01e, tv11);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+2*si2+48) =
          vec_mergel (tv01e, tv11);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2) =
          vec_mergeh (sv11e, sv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2+16) =
          vec_mergel (sv11e, sv01);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2+32) =
          vec_mergeh (tv11e, sv11);
        *(__attribute__((altivec(vector__))) unsigned char *)(out+3*si2+48) =
          vec_mergel (tv11e, sv11);
      } 
    }
    ds += 64;
    dt += 64;
    savg += 64;
    tavg += 64;
    cur += 64;
    cur_uv += ((!(l1 && i > 3)) << 5);
    out += 4*si2;
    motin += 4*si1;
    m3 += 4*si1;
  }
}

void M::foo(unsigned si1, unsigned si2)
{
  unsigned char tavg[3][32*16];
  unsigned char savg[3][32*16];
  unsigned char dtsum[32*16];
  unsigned char dssum[32*16];
  unsigned t16;
  unsigned even = 0;
  unsigned hz = 0;
  unsigned char *cur;
  unsigned char *cur_uv;
  unsigned char *motin;
  unsigned char *m3;
  unsigned char *res;
  unsigned cc;
  unsigned h1;
  T *p = __null;
  int l1;
  int f1;
  bar (savg[cc], tavg[cc], dssum, dtsum, cur, cur_uv, res, motin, m3, even,
       hz, h1, l1, f1, si1, si2, t16,p);
  bar (savg[cc], tavg[cc], dssum, dtsum, cur, cur_uv, res, motin, m3, even,
       hz, h1, l1, f1, si1, si2, t16,p);
}
