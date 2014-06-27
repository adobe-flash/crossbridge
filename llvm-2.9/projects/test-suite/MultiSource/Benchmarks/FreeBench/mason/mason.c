/* Copyright (c) 2000 Tord Hansson */

#define BENCHMARK

#include <stdio.h>
#include <stdlib.h>

typedef struct { int a,b,c,d,bi,ar,g1,g2,g3; } p_type;

static p_type m0u(p_type p) {
  int m[]={0,
	   8,
	   1,
	   2,
	   16,
	   5,
	   13,
	   7,
	   14,
	   9,
	   3,
	   4,
	   11,
	   12,
	   15,
	   10,
	   17,
	   6};
  p_type pu;

  pu.a = m[p.a];
  pu.b = m[p.b];
  pu.c = m[p.c];
  pu.d = m[p.d];
  pu.bi = m[p.bi];
  pu.ar = m[p.ar];
  pu.g1 = m[p.g1];
  pu.g2 = m[p.g2];
  pu.g3 = m[p.g3];
  return pu;
}

static p_type m1u(p_type p) {
  return p;
}

static p_type m2u(p_type p) {
  int m[]={1,
	   2,
	   9,
	   10,
	   4,
	   17,
	   6,
	   0,
	   8,
	   15,
	   11,
	   12,
	   5,
	   13,
	   7,
	   14,
	   3,
	   16};
  p_type pu;

  pu.a = m[p.a];
  pu.b = m[p.b];
  pu.c = m[p.c];
  pu.d = m[p.d];
  pu.bi = m[p.bi];
  pu.ar = m[p.ar];
  pu.g1 = m[p.g1];
  pu.g2 = m[p.g2];
  pu.g3 = m[p.g3];
  return pu;

}

static p_type m0d(p_type p) {
  int m[]={0,
	   2,
	   3,
	   10,
	   11,
	   5,
	   17,
	   7,
	   1,
	   9,
	   15,
	   12,
	   13,
	   6,
	   8,
	   14,
	   4,
	   16};
  p_type pu;

  pu.a = m[p.a];
  pu.b = m[p.b];
  pu.c = m[p.c];
  pu.d = m[p.d];
  pu.bi = m[p.bi];
  pu.ar = m[p.ar];
  pu.g1 = m[p.g1];
  pu.g2 = m[p.g2];
  pu.g3 = m[p.g3];
  return pu;

}

static p_type m1d(p_type p) {
  return p;
}

static p_type m2d(p_type p) {
  int m[]={7,
	   0,
	   1,
	   16,
	   4,
	   12,
	   6,
	   14,
	   8,
	   2,
	   3,
	   10,
	   11,
	   13,
	   15,
	   9,
	   17,
	   5};
  p_type pu;

  pu.a = m[p.a];
  pu.b = m[p.b];
  pu.c = m[p.c];
  pu.d = m[p.d];
  pu.bi = m[p.bi];
  pu.ar = m[p.ar];
  pu.g1 = m[p.g1];
  pu.g2 = m[p.g2];
  pu.g3 = m[p.g3];
  return pu;
}

#define A 0x1
#define B 0x2
#define C 0x4
#define D 0x8
#define BI 0x10
#define AR 0x20
#define G 0x40

static int md(p_type p,int maxdep,int dep,int last);
static int mu(p_type p,int maxdep,int dep,int last) {
  static int near[] = {0x50,0x70,0x70,0x75,0xf,0xf,0xf,0x70,0x70,0x70,0x7d,0x2b,0x2f,0xf,0x50,0x64,0x67,0xf};
#define KKK 4
  /* 2 : {0x40,0x40,0x40,0x60,0x0,0x5,0xa,0x50,0x70,0x70,0x24,0xc,0x9,0xe,0x20,0x10,0x1,0x3}; */
  /* {0,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1};*/
  
  int win;
  if( dep == maxdep - KKK ) {
    if(( B & near[p.b]) == 0) return 0;
    if(( D & near[p.d]) == 0) return 0;
    if(( BI & near[p.bi]) == 0) return 0;
    if(( AR & near[p.ar]) == 0) return 0;
    if(( A & near[p.a]) == 0) return 0;
    if(( C & near[p.c]) == 0) return 0;
    if(( G & near[p.g1]) == 0) return 0;
    if(( G & near[p.g2]) == 0) return 0;
    if(( G & near[p.g3]) == 0) return 0;
  }
  if((p.a == 5) && (p.b == 6) && (p.c == 12) && (p.d == 13) && (p.bi == 14) && (p.ar == 15)  && ((p.g1+p.g2+p.g3) == 3) ) {
    printf("Gul: %d %d %d\n",p.g1,p.g2,p.g3);
    printf("bin+art: %d %d\n",p.bi,p.ar);
    return 1;
  } else {
    if(maxdep <= dep) return 0;

    win = (last == 0 ? 0 : md(m0u(p),maxdep,dep+1,0));
    if(win == 1) {
      putchar('0');
      if((dep % 4) == 0)putchar(' ');
      return win;
    } else {
      win = (last == 1 ? 0 : md(m1u(p),maxdep,dep+1,1));
      if(win == 1) {
	putchar('1');
	if((dep % 4) == 0)putchar(' ');
	return win;
      } else {
	win = (last == 2 ? 0 : md(m2u(p),maxdep,dep+1,2));
	if(win == 1) {
	  putchar('2');
	  if((dep % 4) == 0)putchar(' ');
	}
	return win;
      }
    }
  }
}


static int md(p_type p,int maxdep,int dep,int last) {
  int win;

  if(maxdep <= dep) return 0;

  win = (last == 0 ? 0 : mu(m0d(p),maxdep,dep+1,0));
  if(win == 1) {
    putchar('0');
    if((dep % 4) == 0)putchar(' ');
    return win;
  } else {
    win = (last == 1 ? 0 : mu(m1d(p),maxdep,dep+1,1));
    if(win == 1) {
      putchar('1');
      if((dep % 4) == 0)putchar(' ');
      return win;
    } else {
      win = (last == 2 ? 0 : mu(m2d(p),maxdep,dep+1,2));
      if(win == 1) {
	putchar('2');
	if((dep % 4) == 0)putchar(' ');
      }
      return win;
    }
  }
}

#ifndef BENCHMARK
static void prnear(int k);
#endif

int main(int argc,char *argv[]) {
  int k;
  p_type p;
#ifdef BENCHMARK
  FILE *fp;
#endif

  fprintf(stderr,"Compile date: %s\n", COMPDATE);
  fprintf(stderr,"Compiler switches: %s\n", CFLAGS);

#ifndef BENCHMARK
  if(argc < 10) {
    if(argc == 2) {
      prnear(atoi(argv[1]));
    } else {
      printf("           16 17\n");
      printf("0  1  2  3  4  5  6\n");
      printf("7  8  9 10 11 12 13\n");
      printf("  14 15\n\nEnter A B C D e f G G G :\n");
      printf("G G G   A B\n");
      printf("e f     C D\n");
    }
  } else {
    p.a = atoi(argv[1]);
    p.b = atoi(argv[2]);
    p.c = atoi(argv[3]);
    p.d = atoi(argv[4]);
    p.bi = atoi(argv[5]);
    p.ar = atoi(argv[6]);
    p.g1 = atoi(argv[7]);
    p.g2 = atoi(argv[8]);
    p.g3 = atoi(argv[9]);

    for(k=2;;k+=2) {
      printf("Trying %d\n",k);
      if(mu(p,k,0,-1) == 1) {
	putchar('\n');
	exit(0);
      }
    }
  }
#else /* Run it as a benchmark */
  fp=fopen(argv[1],"r");
  if (fp==NULL) {
    fprintf(stderr,"ERROR in %s: Could not open datafile %s\n",argv[0],argv[1]);
    exit(1);
  }
  
  fscanf(fp,"%d %d %d %d %d %d %d %d %d", 
	 &p.a, &p.b, &p.c, &p.d, &p.bi, &p.ar, &p.g1, &p.g2, &p.g3);
  
  for(k=2;;k+=2) {
    printf("Trying %d\n",k);
    if(mu(p,k,0,-1) == 1) {
      putchar('\n');
      break;
    }
  }
#endif
  
  return 0;
}

static int neard(p_type p,int maxdep,int dep,int last,int near[]);
static int nearu(p_type p,int maxdep,int dep,int last,int near[]) {
  if(maxdep == dep) { 
    near[p.a] |= A;
    near[p.b] |= B;
    near[p.c] |= C;
    near[p.d] |= D;
    near[p.bi] |= BI;
    near[p.ar] |= AR;
    near[p.g1] |= G;
    near[p.g2] |= G;
    near[p.g3] |= G;
    return 0;
  }

  if(last != 0 ) neard(m0u(p),maxdep,dep+1,0,near);
  if(last != 1 ) neard(m1u(p),maxdep,dep+1,1,near);
  if(last != 2 ) neard(m2u(p),maxdep,dep+1,2,near);

  return 0; /* To quiet the compiler... */
}

static int neard(p_type p,int maxdep,int dep,int last,int near[]) {
  if(last != 0 ) nearu(m0d(p),maxdep,dep+1,0,near);
  if(last != 1 ) nearu(m1d(p),maxdep,dep+1,1,near);
  if(last != 2 ) nearu(m2d(p),maxdep,dep+1,2,near);
  return 0;
}

#ifndef BENCHMARK
static void prnear(int k) {
  int i;
  int near[18];
  p_type correct = {5,6,12,13,14,15,0,1,2};

  for(i=0;i<18;i++) near[i] = 0;
  nearu(correct,k,0,-1,near);
  printf("{0x%x",near[0]);
  for(i=1;i<18;i++) printf(",0x%x",near[i]);
  printf("}\n");
}
#endif

/*
  normalläge : skidan i mitten + vänsterflepp nedåt
  numrering : börja på 0; skidan först radvis, sedan vflepp + hflepp

  16 17
  00 01 02 03 04 05 06
  07 08 09 10 11 12 13
  14 15


  Vinst : 
  G G G   A B 
  E F     C D

  Vinster:
  0 1 7 8 
  1 2 8 9
  2 3 9 10
  3 4 10 11
  4 5 11 12
  5 6 12 13

  01 02
  00 08 09 03 16 17 06
  07 14 15 10 04 05 13
  11 12

  Vinster:
  0 8 7 14
  8 9 14 15
  9 3 15 10
  3 16 10 4
  16 17 4 5
  17 6 5 13

*/



