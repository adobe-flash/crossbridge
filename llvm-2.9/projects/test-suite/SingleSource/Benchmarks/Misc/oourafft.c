#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#if !defined(__APPLE__) && !defined(__FreeBSD__) // memalign
#include <malloc.h>
#endif

/* random number generator, 0 <= RND < 1 */
#define RND(p) ((*(p) = (*(p) * 7141 + 54773) % 259200) * (1.0 / 259200.0))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

void makewt(int nw, int *ip, double *w);
void cdft(int, int, double *, int *, double *);
void putdata(int nini, int nend, double *a);
double errorcheck(int nini, int nend, double scale, double *a);

double get_time(void);

#define N 1024
#ifdef SMALL_PROBLEM_SIZE
#define TRIES 5000
#else
#define TRIES 150000
#endif

int main()
{
  int i, j;
  int *ip;
  double *ref, *cmp, *src, *w;
  double t_start, t_end, t_overhead, t_total = 0, err_val;

  /* Measure overhead of get_time() call */
  t_start = get_time();
  t_end = get_time();
  t_overhead = t_end - t_start;

  /* Prepare aux data */
#if !defined(__APPLE__) && !defined(__FreeBSD__) /* Darwin always 16-byte aligns malloc data */
  ip = memalign(16, sqrt(N)*sizeof(int));
  w  = memalign(16, 2*N*5/4*sizeof(double));
#else
  ip = malloc(sqrt(N)*sizeof(int));
  w  = malloc(2*N*5/4*sizeof(double));
#endif
  makewt(N >> 1, ip, w);
  
  /* Allocate buffers */
#if !defined(__APPLE__) && !defined(__FreeBSD__) /* Darwin always 16-byte aligns malloc data */
  ref = memalign(16, 2*N*sizeof(double));
  cmp = memalign(16, 2*N*sizeof(double));
  src = memalign(16, 2*N*sizeof(double));
#else
  ref = malloc(2*N*sizeof(double));
  cmp = malloc(2*N*sizeof(double));
  src = malloc(2*N*sizeof(double));
#endif
  
  /* Perform sanity check of FFT */
  putdata(0, 2*N - 1, ref);
  cdft(2*N,  1, ref, ip, w);
  cdft(2*N, -1, ref, ip, w);
  err_val = errorcheck(0, 2*N - 1, 1.0/N, ref);
  if (fabs(err_val) > 1e-10) {
    printf("FFT sanity check failed! Difference is: %le\n", err_val);
    abort();
  }
  
  /* Prepare reference sequence */
  memset(ref, 0, 2*N*sizeof(double));
  putdata(0, N-1, ref);
  cdft(2*N, 1, ref, ip, w);
  for (j=0; j<N; ++j)
    ref[2*j+1] = -ref[2*j+1];
  
  /*printf("Doing %d correlations (%d FFTs of size %d)\n", TRIES, 2*TRIES, N);*/
  memset(src, 0, 2*N*sizeof(double));
  putdata(0, N-1, src);

  t_start = get_time();
  for (i=0; i<TRIES; ++i) {
    int k;

    memcpy(cmp, src, 2*N*sizeof(double));
    cdft(2*N, 1, cmp, ip, w);

    for (k=0; k<N; ++k) {
      double re1 = cmp[2*k];
      double re2 = ref[2*k];
      double im1 = cmp[2*k+1];
      double im2 = ref[2*k+1];
      cmp[2*k]   = re1*re2 - im1*im2;
      cmp[2*k+1] = re1*im2 + im1*re2;
    }

    cdft(2*N, -1, cmp, ip, w);
  }
  t_end = get_time();
  t_total += t_end - t_start - t_overhead; 

  for (j=0; j<N; ++j) {
    printf("%e %e\n",
           (fabs(cmp[2*j]) > 1e-9 ? cmp[2*j] : 0),
           (fabs(cmp[2*j+1]) > 1e-9 ? cmp[2*j+1] : 0));
  }
  /*printf("Overall time: %le, for single correlation: %le\n", t_total, t_total/TRIES);*/

  free(ref);
  free(w);
  free(ip);

  free(cmp);
  free(src);
  
  return 0;
}

void putdata(int nini, int nend, double *a)
{
  int j, seed = 0;

  for (j = nini; j <= nend; j++)
    a[j] = RND(&seed);
}

double errorcheck(int nini, int nend, double scale, double *a)
{
  int j, seed = 0;
  double err = 0, e;

  for (j = nini; j <= nend; j++) {
    e = RND(&seed) - a[j] * scale;
    err = MAX(err, fabs(e));
  }
  return err;
}

#ifdef WIN32
#include <windows.h>

static LARGE_INTEGER t = {0, 0};

double get_time(void)
{
  LARGE_INTEGER l;
  if (t.QuadPart == 0)
    QueryPerformanceFrequency(&t);
  QueryPerformanceCounter(&l);
  return (double)l.QuadPart / (double)t.QuadPart;
}

#else
#include <sys/time.h>

double get_time(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
}

#endif

void makewt(int nw, int *ip, double *w);
static void bitrv2(int n, int *ip, double *a);
static void bitrv2conj(int n, int *ip, double *a);
static void cftfsub(int n, double *a, double *w);
static void cftbsub(int n, double *a, double *w);
static inline void cft1st(int n, double *a, double *w);
static inline void cftmdl(int n, int l, double *a, double *w);

void cdft(int n, int isgn, double *a, int *ip, double *w)
{    
  if (n > 4) {
    if (isgn >= 0) {
      bitrv2(n, ip, a);
      cftfsub(n, a, w);
    } else {
      bitrv2conj(n, ip, a);
      cftbsub(n, a, w);
    }
  } else if (n == 4) {
    cftfsub(n, a, w);
  }
}

/* -------- initializing routines -------- */

#include <math.h>

void makewt(int nw, int *ip, double *w)
{
  int j, nwh;
  double delta, x, y;
    
  if (nw > 2) {
    nwh = nw >> 1;
    delta = atan(1.0) / nwh;
    w[0] = 1;
    w[1] = 0;
    w[nwh] = cos(delta * nwh);
    w[nwh + 1] = w[nwh];
    if (nwh > 2) {
      for (j = 2; j < nwh; j += 2) {
        x = cos(delta * j);
        y = sin(delta * j);
        w[j] = x;
        w[j + 1] = y;
        w[nw - j] = y;
        w[nw - j + 1] = x;
      }
      bitrv2(nw, ip, w);
    }
  }
}

/* -------- child routines -------- */

void bitrv2(int n, int *ip, double *a)
{
  int j, j1, k, k1, l, m, m2;
  double xr, xi, yr, yi;
    
  ip[0] = 0;
  l = n;
  m = 1;
  while ((m << 3) < l) {
    l >>= 1;
    for (j = 0; j < m; j++) {
      ip[m + j] = ip[j] + l;
    }
    m <<= 1;
  }
  m2 = 2 * m;
  if ((m << 3) == l) {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 2 * j + ip[k];
        k1 = 2 * k + ip[j];
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += 2 * m2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 -= m2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += 2 * m2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      j1 = 2 * k + m2 + ip[k];
      k1 = j1 + m2;
      xr = a[j1];
      xi = a[j1 + 1];
      yr = a[k1];
      yi = a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
    }
  } else {
    for (k = 1; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 2 * j + ip[k];
        k1 = 2 * k + ip[j];
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += m2;
        xr = a[j1];
        xi = a[j1 + 1];
        yr = a[k1];
        yi = a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
    }
  }
}


static void bitrv2conj(int n, int *ip, double *a)
{
  int j, j1, k, k1, l, m, m2;
  double xr, xi, yr, yi;
    
  ip[0] = 0;
  l = n;
  m = 1;
  while ((m << 3) < l) {
    l >>= 1;
    for (j = 0; j < m; j++) {
      ip[m + j] = ip[j] + l;
    }
    m <<= 1;
  }
  m2 = 2 * m;
  if ((m << 3) == l) {
    for (k = 0; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 2 * j + ip[k];
        k1 = 2 * k + ip[j];
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += 2 * m2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 -= m2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += 2 * m2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 2 * k + ip[k];
      a[k1 + 1] = -a[k1 + 1];
      j1 = k1 + m2;
      k1 = j1 + m2;
      xr = a[j1];
      xi = -a[j1 + 1];
      yr = a[k1];
      yi = -a[k1 + 1];
      a[j1] = yr;
      a[j1 + 1] = yi;
      a[k1] = xr;
      a[k1 + 1] = xi;
      k1 += m2;
      a[k1 + 1] = -a[k1 + 1];
    }
  } else {
    a[1] = -a[1];
    a[m2 + 1] = -a[m2 + 1];
    for (k = 1; k < m; k++) {
      for (j = 0; j < k; j++) {
        j1 = 2 * j + ip[k];
        k1 = 2 * k + ip[j];
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
        j1 += m2;
        k1 += m2;
        xr = a[j1];
        xi = -a[j1 + 1];
        yr = a[k1];
        yi = -a[k1 + 1];
        a[j1] = yr;
        a[j1 + 1] = yi;
        a[k1] = xr;
        a[k1 + 1] = xi;
      }
      k1 = 2 * k + ip[k];
      a[k1 + 1] = -a[k1 + 1];
      a[k1 + m2 + 1] = -a[k1 + m2 + 1];
    }
  }
}


void cftfsub(int n, double *a, double *w)
{
  int j, j1, j2, j3, l;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
  l = 2;
  if (n > 8) {
    cft1st(n, a, w);
    l = 8;
    while ((l << 2) < n) {
      cftmdl(n, l, a, w);
      l <<= 2;
    }
  }
  if ((l << 2) == n) {
    for (j = 0; j < l; j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = a[j + 1] + a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = a[j + 1] - a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i + x2i;
      a[j2] = x0r - x2r;
      a[j2 + 1] = x0i - x2i;
      a[j1] = x1r - x3i;
      a[j1 + 1] = x1i + x3r;
      a[j3] = x1r + x3i;
      a[j3 + 1] = x1i - x3r;
    }
  } else {
    for (j = 0; j < l; j += 2) {
      j1 = j + l;
      x0r = a[j] - a[j1];
      x0i = a[j + 1] - a[j1 + 1];
      a[j] += a[j1];
      a[j + 1] += a[j1 + 1];
      a[j1] = x0r;
      a[j1 + 1] = x0i;
    }
  }
}


void cftbsub(int n, double *a, double *w)
{
  int j, j1, j2, j3, l;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
  l = 2;
  if (n > 8) {
    cft1st(n, a, w);
    l = 8;
    while ((l << 2) < n) {
      cftmdl(n, l, a, w);
      l <<= 2;
    }
  }
  if ((l << 2) == n) {
    for (j = 0; j < l; j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = -a[j + 1] - a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = -a[j + 1] + a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i - x2i;
      a[j2] = x0r - x2r;
      a[j2 + 1] = x0i + x2i;
      a[j1] = x1r - x3i;
      a[j1 + 1] = x1i - x3r;
      a[j3] = x1r + x3i;
      a[j3 + 1] = x1i + x3r;
    }
  } else {
    for (j = 0; j < l; j += 2) {
      j1 = j + l;
      x0r = a[j] - a[j1];
      x0i = -a[j + 1] + a[j1 + 1];
      a[j] += a[j1];
      a[j + 1] = -a[j + 1] - a[j1 + 1];
      a[j1] = x0r;
      a[j1 + 1] = x0i;
    }
  }
}


void cft1st(int n, double *a, double *w)
{
  int j, k1, k2;
  double wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
  x0r = a[0] + a[2];
  x0i = a[1] + a[3];
  x1r = a[0] - a[2];
  x1i = a[1] - a[3];
  x2r = a[4] + a[6];
  x2i = a[5] + a[7];
  x3r = a[4] - a[6];
  x3i = a[5] - a[7];
  a[0] = x0r + x2r;
  a[1] = x0i + x2i;
  a[4] = x0r - x2r;
  a[5] = x0i - x2i;
  a[2] = x1r - x3i;
  a[3] = x1i + x3r;
  a[6] = x1r + x3i;
  a[7] = x1i - x3r;
  wk1r = w[2];
  x0r = a[8] + a[10];
  x0i = a[9] + a[11];
  x1r = a[8] - a[10];
  x1i = a[9] - a[11];
  x2r = a[12] + a[14];
  x2i = a[13] + a[15];
  x3r = a[12] - a[14];
  x3i = a[13] - a[15];
  a[8] = x0r + x2r;
  a[9] = x0i + x2i;
  a[12] = x2i - x0i;
  a[13] = x0r - x2r;
  x0r = x1r - x3i;
  x0i = x1i + x3r;
  a[10] = wk1r * (x0r - x0i);
  a[11] = wk1r * (x0r + x0i);
  x0r = x3i + x1r;
  x0i = x3r - x1i;
  a[14] = wk1r * (x0i - x0r);
  a[15] = wk1r * (x0i + x0r);
  k1 = 0;
  for (j = 16; j < n; j += 16) {
    k1 += 2;
    k2 = 2 * k1;
    wk2r = w[k1];
    wk2i = w[k1 + 1];
    wk1r = w[k2];
    wk1i = w[k2 + 1];
    wk3r = wk1r - 2 * wk2i * wk1i;
    wk3i = 2 * wk2i * wk1r - wk1i;
    x0r = a[j] + a[j + 2];
    x0i = a[j + 1] + a[j + 3];
    x1r = a[j] - a[j + 2];
    x1i = a[j + 1] - a[j + 3];
    x2r = a[j + 4] + a[j + 6];
    x2i = a[j + 5] + a[j + 7];
    x3r = a[j + 4] - a[j + 6];
    x3i = a[j + 5] - a[j + 7];
    a[j] = x0r + x2r;
    a[j + 1] = x0i + x2i;
    x0r -= x2r;
    x0i -= x2i;
    a[j + 4] = wk2r * x0r - wk2i * x0i;
    a[j + 5] = wk2r * x0i + wk2i * x0r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j + 2] = wk1r * x0r - wk1i * x0i;
    a[j + 3] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j + 6] = wk3r * x0r - wk3i * x0i;
    a[j + 7] = wk3r * x0i + wk3i * x0r;
    wk1r = w[k2 + 2];
    wk1i = w[k2 + 3];
    wk3r = wk1r - 2 * wk2r * wk1i;
    wk3i = 2 * wk2r * wk1r - wk1i;
    x0r = a[j + 8] + a[j + 10];
    x0i = a[j + 9] + a[j + 11];
    x1r = a[j + 8] - a[j + 10];
    x1i = a[j + 9] - a[j + 11];
    x2r = a[j + 12] + a[j + 14];
    x2i = a[j + 13] + a[j + 15];
    x3r = a[j + 12] - a[j + 14];
    x3i = a[j + 13] - a[j + 15];
    a[j + 8] = x0r + x2r;
    a[j + 9] = x0i + x2i;
    x0r -= x2r;
    x0i -= x2i;
    a[j + 12] = -wk2i * x0r - wk2r * x0i;
    a[j + 13] = -wk2i * x0i + wk2r * x0r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j + 10] = wk1r * x0r - wk1i * x0i;
    a[j + 11] = wk1r * x0i + wk1i * x0r;
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j + 14] = wk3r * x0r - wk3i * x0i;
    a[j + 15] = wk3r * x0i + wk3i * x0r;
  }
}


void cftmdl(int n, int l, double *a, double *w)
{
  int j, j1, j2, j3, k, k1, k2, m, m2;
  double wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
  double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
  m = l << 2;
  for (j = 0; j < l; j += 2) {
    j1 = j + l;
    j2 = j1 + l;
    j3 = j2 + l;
    x0r = a[j] + a[j1];
    x0i = a[j + 1] + a[j1 + 1];
    x1r = a[j] - a[j1];
    x1i = a[j + 1] - a[j1 + 1];
    x2r = a[j2] + a[j3];
    x2i = a[j2 + 1] + a[j3 + 1];
    x3r = a[j2] - a[j3];
    x3i = a[j2 + 1] - a[j3 + 1];
    a[j] = x0r + x2r;
    a[j + 1] = x0i + x2i;
    a[j2] = x0r - x2r;
    a[j2 + 1] = x0i - x2i;
    a[j1] = x1r - x3i;
    a[j1 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
  }
  wk1r = w[2];
  for (j = m; j < l + m; j += 2) {
    j1 = j + l;
    j2 = j1 + l;
    j3 = j2 + l;
    x0r = a[j] + a[j1];
    x0i = a[j + 1] + a[j1 + 1];
    x1r = a[j] - a[j1];
    x1i = a[j + 1] - a[j1 + 1];
    x2r = a[j2] + a[j3];
    x2i = a[j2 + 1] + a[j3 + 1];
    x3r = a[j2] - a[j3];
    x3i = a[j2 + 1] - a[j3 + 1];
    a[j] = x0r + x2r;
    a[j + 1] = x0i + x2i;
    a[j2] = x2i - x0i;
    a[j2 + 1] = x0r - x2r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j1] = wk1r * (x0r - x0i);
    a[j1 + 1] = wk1r * (x0r + x0i);
    x0r = x3i + x1r;
    x0i = x3r - x1i;
    a[j3] = wk1r * (x0i - x0r);
    a[j3 + 1] = wk1r * (x0i + x0r);
  }
  k1 = 0;
  m2 = 2 * m;
  for (k = m2; k < n; k += m2) {
    k1 += 2;
    k2 = 2 * k1;
    wk2r = w[k1];
    wk2i = w[k1 + 1];
    wk1r = w[k2];
    wk1i = w[k2 + 1];
    wk3r = wk1r - 2 * wk2i * wk1i;
    wk3i = 2 * wk2i * wk1r - wk1i;
    for (j = k; j < l + k; j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = a[j + 1] + a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = a[j + 1] - a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i + x2i;
      x0r -= x2r;
      x0i -= x2i;
      a[j2] = wk2r * x0r - wk2i * x0i;
      a[j2 + 1] = wk2r * x0i + wk2i * x0r;
      x0r = x1r - x3i;
      x0i = x1i + x3r;
      a[j1] = wk1r * x0r - wk1i * x0i;
      a[j1 + 1] = wk1r * x0i + wk1i * x0r;
      x0r = x1r + x3i;
      x0i = x1i - x3r;
      a[j3] = wk3r * x0r - wk3i * x0i;
      a[j3 + 1] = wk3r * x0i + wk3i * x0r;
    }
    wk1r = w[k2 + 2];
    wk1i = w[k2 + 3];
    wk3r = wk1r - 2 * wk2r * wk1i;
    wk3i = 2 * wk2r * wk1r - wk1i;
    for (j = k + m; j < l + (k + m); j += 2) {
      j1 = j + l;
      j2 = j1 + l;
      j3 = j2 + l;
      x0r = a[j] + a[j1];
      x0i = a[j + 1] + a[j1 + 1];
      x1r = a[j] - a[j1];
      x1i = a[j + 1] - a[j1 + 1];
      x2r = a[j2] + a[j3];
      x2i = a[j2 + 1] + a[j3 + 1];
      x3r = a[j2] - a[j3];
      x3i = a[j2 + 1] - a[j3 + 1];
      a[j] = x0r + x2r;
      a[j + 1] = x0i + x2i;
      x0r -= x2r;
      x0i -= x2i;
      a[j2] = -wk2i * x0r - wk2r * x0i;
      a[j2 + 1] = -wk2i * x0i + wk2r * x0r;
      x0r = x1r - x3i;
      x0i = x1i + x3r;
      a[j1] = wk1r * x0r - wk1i * x0i;
      a[j1 + 1] = wk1r * x0i + wk1i * x0r;
      x0r = x1r + x3i;
      x0i = x1i - x3r;
      a[j3] = wk3r * x0r - wk3i * x0i;
      a[j3 + 1] = wk3r * x0i + wk3i * x0r;
    }
  }
}

