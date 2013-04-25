/* APPLE LOCAL file 4115609 */
/* { dg-do compile { target i?86-*-darwin* } } */
/* { dg-require-effective-target ilp32 } */
/* { dg-options "-O3 -mdynamic-no-pic -fomit-frame-pointer" } */
/* { dg-final { scan-assembler-times "movl\t*" 55} } */
/* { dg-final { scan-assembler-not "sall" } } */
#define px(a, b) (((((unsigned int)(a)) * (unsigned int)(b) * 0x10101) \
        + 0x800000) >> 24)
typedef struct {
  int t, l, h, w;
} rect;

typedef struct {
  unsigned char r, g, b, al;
} pl;

typedef struct {
  pl  *ba;
  int  rc;
} cv;

typedef struct {
  rect ar;
  cv   s1;
  cv   s2;
  cv   ds;
  unsigned char op;
} rec;

int foo (rec *da) {
  int r1, r1t, dr, bm, rt, r2t, h, v, result = 0; 
  pl *b1, *b2, *db, *s1, *s2, *dl; 
  unsigned char al, r, g, b, ia, op = da->op;
  b1 = da->s1.ba;
  b2 = da->s2.ba;
  db = da->ds.ba;
  r1 = da->s1.rc;
  r2t = da->s2.rc;
  dr = da->ds.rc;
  bm = da->ar.t + da->ar.h; 
  rt = da->ar.l + da->ar.w; 
  for (v = da->ar.t; !result && (v < bm); v++) { 
    s1 = b1 + v * r1 + da->ar.l; 
    s2 = b2 + v * r2t + da->ar.l; 
    dl = db + v * dr + da->ar.l; 
      for (h=da->ar.l; h<rt; h++, s1++, s2++, dl++) {
        r  = s2->r;
        g = s2->g;
        b = s2->b;
        al = px (s2->al, op);
        ia = 255-al;
        dl->al = s1->al + px (255-s1->al, px (s2->al,op));
        dl->r = px (ia, s1->r) + px (al, s1->r > r ? r : s1->r);
  	dl->g = px (ia, s1->g) + px (al, s1->g > g ? g : s1->g );
        dl->b = px (ia, s1->b) + px (al, s1->b > b ? b : s1->b );
      } 
    }	
  return result;
}
