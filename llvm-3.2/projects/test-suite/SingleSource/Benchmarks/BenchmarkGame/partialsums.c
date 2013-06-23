//  The Computer Language Shootout
//  http://shootout.alioth.debian.org/
//  contributed by Greg Buchholz
//
//  compile with:  -O3 -msse2 -lm

#include<math.h>
#include<stdio.h>

typedef double v2df __attribute__ ((vector_size (16)));

v2df make_vec(double a, double b)
{
    v2df v;
    double *tmp;
    tmp = (double *)&v; *(tmp) = a; *(tmp+1) = b;
    return v;
}

double sum_vec(v2df x)
{
    double *tmp = (double *)&x;
    return *(tmp) + *(tmp+1);
}

int main(int argc, char* argv[])
{
    double  twoThrd = 0, sqrts = 0, Flint = 0, Cookson = 0;
    v2df    Harmonic, zeta, poly, alt, Gregory;
    v2df    zero, one, two, init, m_one, kv, av;

    double  k, k3, s, c;
    int n;  n = 2500000;

    zero  = make_vec( 0.0,  0.0);  one   = make_vec( 1.0,  1.0);
    two   = make_vec( 2.0,  2.0);  m_one = make_vec(-1.0, -1.0);
    init  = make_vec( 1.0,  2.0);  av    = make_vec( 1.0, -1.0);

    Harmonic = zeta = poly = alt = Gregory = zero;

    for (k=1; k<=n; k++)
    {
        twoThrd += pow(2.0/3.0, k-1);
        sqrts   += 1.0/sqrt(k);
              k3 = k*k*k;
              s  = sin(k); c = cos(k);
        Flint   += 1.0/(k3 * s*s);
        Cookson += 1.0/(k3 * c*c);
    }

    for (kv=init; *(double *)(&kv)<=n; kv+=two)
    {
        poly    += one /(kv*(kv+one));
        Harmonic+= one / kv;
        zeta    += one /(kv*kv);
        alt     +=  av / kv;
        Gregory +=  av /(two*kv - one);
    }

#define psum(name,num) printf("%.9f\t%s\n",num,name)
    psum("(2/3)^k",           twoThrd); psum("k^-0.5",      sqrts);
    psum("1/k(k+1)",    sum_vec(poly)); psum("Flint Hills", Flint);
    psum("Cookson Hills",     Cookson); psum("Harmonic", sum_vec(Harmonic));
    psum("Riemann Zeta",sum_vec(zeta)); psum("Alternating Harmonic",sum_vec(alt));
    psum("Gregory",  sum_vec(Gregory));

    return 0;
}