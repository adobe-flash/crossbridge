/*
    fftbench
    Standard C++ version
    10 May 2003

    Written by Scott Robert Ladd.
    No rights reserved. This is public domain software, for use by anyone.

    A number-crunching benchmark that can be used as a fitness test for
    evolving optimal compiler options via genetic algorithm.

    A simplified version of the FFT component from my standard libcoyote
    library, this benchmark test FFT performance on complex<double> values.

    Note that the code herein is design for the purpose of testing 
    computational performance; error handling and other such "niceties"
    is virtually non-existent.

    Actual benchmark results can be found at:
            http://www.coyotegulch.com

    Please do not use this information or algorithm in any way that might
    upset the balance of the universe or to produce imaginary friends for
    imaginary numbers.
*/

#include <cmath>
#include <complex>
#include <ctime>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <stdexcept>

using namespace std;

//---------------------------------------------------------------------------
// number of loops to perform
#if defined(ACOVEA)
#if defined(arch_pentium4)
static const int TEST_SIZE   =   262144;
#else
static const int TEST_SIZE   =   131072;
#endif
#else
#ifdef SMALL_PROBLEM_SIZE
static const int TEST_SIZE   =  50000;
#else
static const int TEST_SIZE   =  2097152/4;
#endif
#endif

//---------------------------------------------------------------------------
// embedded random number generator; ala Park and Miller
static double random_double()
{
    static       long seed = 1325;
    static const long IA   = 16807;
    static const long IM   = 2147483647;
    static const double AM = 4.65661287525E-9;
    static const long IQ   = 127773;
    static const long IR   = 2836;
    static const long MASK = 123459876;

    long k;
    double result;
    
    seed ^= MASK;
    k = seed / IQ;
    seed = IA * (seed - k * IQ) - IR * k;
    
    if (seed < 0)
        seed += IM;
    
    result = AM * seed;
    seed ^= MASK;
    
    return result;
}

//---------------------------------------------------------------------------
template <class T>
class polynomial
{
    public:
        // creation constructor, uninitialized (coefficients will be unknown values)
        polynomial(size_t degree);
    
        // creation constructor, with initialization from array
        polynomial(size_t degree, const T * coefficients);
    
        // creation constructor, with single-value initialization constructor
        polynomial(size_t degree, const T & value);
    
        // copy constructor
        polynomial(const polynomial<T> & source);
        
        // destructor
        virtual ~polynomial();
        
        // assignment
        polynomial<T> & operator = (const polynomial<T> & source);
        
        // initialize to specific value
        void initialize(const T & value = T(0));
    
        // increase polynomial length
        polynomial<T> & stretch(size_t degrees);
        
        // interrogate for degree
        size_t degree() const
        {
            return m_degree;
        }
        
        // get specific coefficient
        T get(size_t term) const;
        
        T & operator [] (size_t term);
        
        // evaluate for a specific value
        T operator() (const T & x) const;
        
        // unitary operators
        polynomial<T> operator - () const;
        polynomial<T> operator + () const;
        
        // binary mathematical operators
        polynomial<T> operator + (const polynomial<T> & poly) const;
        polynomial<T> operator - (const polynomial<T> & poly) const;
        
        // constant required by FFT routines
        static const complex<T> PI2I;

        // returns largest power of two that holds n
        static size_t log2(size_t n);

        // reverses a sequence of bits
        static size_t flip_bits(size_t k, size_t bits);

        // stretches the length of a polynomial to a power of two
        size_t stretch_fft();

        // performs a reverse-bit copy of a polynomial<T> into a new polynomial< complex<T> >
        static polynomial< complex<T> > bit_reverse(const polynomial<T> & poly);
        static polynomial< complex<T> > bit_reverse(const polynomial< complex<T> > & poly);

        // Fast Fourier Transform of polynomial<T> to polynomial< complex<T> >
        static polynomial< complex<T> > fft(const polynomial<T> & poly);

        // inverse FFT of polynomial< complex<T> > to polynomial<T>
        static polynomial< complex<T> > inverse_fft(const polynomial< complex<T> > & poly);

        // multiplication via FFT
        polynomial<T> operator * (const polynomial<T> & poly) const;
        
        // shorthand mathematical operators
        polynomial<T> & operator += (const polynomial<T> & poly);
        polynomial<T> & operator -= (const polynomial<T> & poly);
        polynomial<T> & operator *= (const polynomial<T> & poly);
        
    protected:
        // coefficients
        T * m_coeff;
    
        // number of terms
        size_t m_degree;
        
        // acquire resources
        void acquire();
        
        // release resources
        void release();
        
        // deep copy
        void deep_copy(const T * source);
};

// acquire resources
template <typename T>
inline void polynomial<T>::acquire()
{
    m_coeff = new T [m_degree];
}

// release resources
template <typename T>
inline void polynomial<T>::release()
{
    delete [] m_coeff;
}

// deep copy
template <typename T>
inline void polynomial<T>::deep_copy(const T * source)
{
    for (size_t n = 0; n < m_degree; ++n)
        m_coeff[n] = source[n];
}

// initialize to specific value
template <typename T>
inline void polynomial<T>::initialize(const T & value)
{
    for (size_t n = 0; n < m_degree; ++n)
        m_coeff[n] = value;
}
    
// creation constructor, uninitialized (coefficients will be unknown values)
template <typename T>
polynomial<T>::polynomial(size_t degree)
  : m_coeff(NULL),
    m_degree(degree)
{
    acquire();
}

// creation constructor, with possible initialization from array
template <typename T>
polynomial<T>::polynomial(size_t degree, const T * coefficients)
  : m_coeff(NULL),
    m_degree(degree)
{
    acquire();
    
    if (coefficients != NULL)
        deep_copy(coefficients);
}

// creation constructor, with single-value initialization constructor
template <typename T>
polynomial<T>::polynomial(size_t degree, const T & value)
  : m_coeff(NULL),
    m_degree(degree)
{
    acquire();
    initialize(value);
}
    
// copy constructor
template <typename T>
polynomial<T>::polynomial(const polynomial<T> & source)
  : m_coeff(NULL),
    m_degree(source.m_degree)
{
    acquire();
    deep_copy(source.m_coeff);
}

// destructor
template <typename T>
polynomial<T>::~polynomial()
{
    release();
}

// assignment
template <typename T>
polynomial<T> & polynomial<T>::operator = (const polynomial<T> & source)
{
    if (m_degree != source.m_degree)
    {
        release();
    
        m_degree = source.m_degree;
        acquire();
    }
    
    deep_copy(source.m_coeff);
    
    return *this;
}

// increase polynomial length
template <typename T>
polynomial<T> & polynomial<T>::stretch(size_t degrees)
{
    if (degrees != 0)
    {
        T * old_coeff = m_coeff;
        size_t old_degree = m_degree;
    
        m_degree += degrees;
        acquire();
        
        size_t n = 0;
        
        for (; n < old_degree; ++n)
            m_coeff[n] = old_coeff[n];
        
        for (; n < m_degree; ++n)
            m_coeff[n] = T(0);
    }
    
    return *this;
}

// get specific coefficient
template <typename T>
T polynomial<T>::get(size_t term) const
{
    return m_coeff[term];
}

template <typename T>
T & polynomial<T>::operator [] (size_t term)
{
    return m_coeff[term];
}

// evaluate for a specific value
template <typename T>
T polynomial<T>::operator() (const T & x) const
{
    // using Horner's Rule
    T y = m_coeff[m_degree - 1];
    
    size_t i = m_degree - 2;
    
    while (true)
    {
        y = x * y + m_coeff[i];
        
        if (i > 0)
            --i;
        else
            break;
    }
    
    return y;
}

// unitary operators
template <typename T>
polynomial<T> polynomial<T>::operator - () const
{
    polynomial<T> result(m_degree); // uninitialized
    
    for (size_t n = 0; n < m_degree; ++n)
        result.m_coeff[n] = -m_coeff[n];
    
    return result;
}

template <typename T>
polynomial<T> polynomial<T>::operator + () const
{
    return polynomial<T>(*this); 
}

// binary mathematical operators
template <typename T>
polynomial<T> polynomial<T>::operator + (const polynomial<T> & poly) const
{
    if (m_degree >= poly.m_degree)
    {
        polynomial<T> result(*this); 
    
        for (size_t n = 0; n < m_degree; ++n)
            result.m_coeff[n] += poly.m_coeff[n];
    
        return result;
    }
    else
    {
        polynomial<T> result(poly); 
    
        for (size_t n = 0; n < m_degree; ++n)
            result.m_coeff[n] += m_coeff[n];
    
        return result;
    }
}

template <typename T>
polynomial<T> polynomial<T>::operator - (const polynomial<T> & poly) const
{
    if (m_degree >= poly.m_degree)
    {
        polynomial<T> result(*this); 
    
        for (size_t n = 0; n < m_degree; ++n)
            result.m_coeff[n] -= poly.m_coeff[n];
    
        return result;
    }
    else
    {
        polynomial<T> result(poly); 
    
        for (size_t n = 0; n < m_degree; ++n)
            result.m_coeff[n] -= m_coeff[n];
    
        return result;
    }
}

// constant required by FFT routines
template <typename T>
const complex<T> polynomial<T>::PI2I(0.0,6.283185307179586);

// returns largest power of two that holds n
template <typename T>
size_t polynomial<T>::log2(size_t n)
{
    // returns 1 if n == 0!
    size_t x = 1, c = 0;

    while (x < n)
    {
        ++c;
        x <<= 1;

        if (x == 0)
            break;
    }

    return c;
}

// reverses a sequence of bits
template <typename T>
size_t polynomial<T>::flip_bits(size_t k, size_t bits)
{
    size_t lm = 1 << (bits - 1);
    size_t rm = 1;
    size_t  r = 0;

    while (lm)
    {
        if (k & rm)
            r |= (lm);

        lm >>= 1;
        rm <<= 1;
    }

    return r;
}

// stretches the length of a polynomial to a power of two
template <class T>
size_t polynomial<T>::stretch_fft()
{
    size_t n = 1;

    while (true)
    {
        if (m_degree <= n)
            break;

        n <<= 1;

        if (n == 0)
            throw overflow_error("overflow in fft polynomial stretch");
    }

    n <<= 1;
    n -= m_degree;

    if (n > 0)
        stretch(n);

    return n;
}

// performs a reverse-bit copy of a polynomial<T> into a new polynomial< complex<T> >
template <typename T>
polynomial< complex<T> > polynomial<T>::bit_reverse(const polynomial<T> & poly)
{
    size_t b = log2(poly.degree());

    polynomial< complex<T> > result(poly.degree());

    for (size_t n = 0; n < poly.degree(); ++n)
        result[flip_bits(n,b)] = poly.get(n);

    return result;
}

// performs a reverse-bit copy of a polynomial<T> into a new polynomial< complex<T> >
template <typename T>
polynomial< complex<T> > polynomial<T>::bit_reverse(const polynomial< complex<T> > & poly)
{
    size_t b = log2(poly.degree());
    
    polynomial< complex<T> > result(poly.degree());

    for (size_t n = 0; n < poly.degree(); ++n)
        result[flip_bits(n,b)] = poly.get(n);

    return result;
}

// Fast Fourier Transform of polynomial<T> to polynomial< complex<T> >
template <typename T>
polynomial< complex<T> > polynomial<T>::fft(const polynomial<T> & poly)
{
    size_t nl = log2(poly.degree());
    size_t j, k, m, m2, s;
    complex<T> wm, w, t, u;
    
    polynomial< complex<T> > result = bit_reverse(poly);

    m  = 2;
    m2 = 1;

    for (s = 0; s < nl; ++s)
    {
        wm = exp(PI2I / complex<T>(m));
        w  = 1.0;

        for (j = 0; j <= (m2 - 1); ++j)
        {
            for (k = j; k <= poly.degree() - 1; k += m)
            {
                t = w * result[k + m2];
                u = result[k];
                result[k] = u + t;
                result[k + m2] = u - t;
            }

            w *= wm;
        }

        m  <<= 1;
        m2 <<= 1;
    }

    return result;
}

// inverse FFT of polynomial< complex<T> > to polynomial<T>
template <typename T>
polynomial< complex<T> > polynomial<T>::inverse_fft(const polynomial< complex<T> > & poly)
{
    size_t nl = log2(poly.degree());
    size_t j, k, m, m2, s;
    complex<T> wm, w, t, u;
    
    polynomial< complex<T> > result = bit_reverse(poly);

    m  = 2;
    m2 = 1;

    for (s = 0; s < nl; ++s)
    {
        wm = exp(-PI2I / complex<T>(m));
        w  = 1.0;

        for (j = 0; j <= (m2 - 1); ++j)
        {
            for (k = j; k <= poly.degree() - 1; k += m)
            {
                t = w * result[k + m2];
                u = result[k];
                result[k] = u + t;
                result[k + m2] = u - t;
            }

            w *= wm;
        }

        m  <<= 1;
        m2 <<= 1;
    }

    for (j = 0; j < poly.degree(); ++j)
        result[j] /= double(poly.degree());

    return result;
}

// multiplication by FFT
template <typename T>
polynomial<T> polynomial<T>::operator * (const polynomial<T> & poly) const
{
#if defined(BRUTE_FORCE)
    // brute force algorithm
    if (m_degree != poly.m_degree)
        throw domain_error("can not multiply two polynomials with different lengths");
    
    polynomial<T> result(2 * m_degree - 1, T(0));
    
    for (size_t n1 = 0; n1 < m_degree; ++n1)
        for (size_t n2 = 0; n2 < m_degree; ++n2)
            result.m_coeff[n1 + n2] += m_coeff[n1] * poly.m_coeff[n2];
#else
    // duplicate p1 and p2 to preserve original objects
    polynomial<T> a1(*this);
    polynomial<T> a2(poly);
    
    // expand polynomials to next-largest power of two
    if (a1.degree() > a2.degree())
        a2.stretch(a1.stretch_fft());
    else
        a1.stretch(a2.stretch_fft());
    
    // FFT polynomials
    polynomial< complex<T> > dft1 = fft(a1);
    polynomial< complex<T> > dft2 = fft(a2);

    // multiply coefficients
    size_t n2 = a1.degree();
    
    for (size_t k = 0; k < n2; ++k)
        dft1[k] *= dft2[k];

    // inverse DFT to obtain result
    dft2 = inverse_fft(dft1);

    // convert back to complex<T>
    --n2;
    polynomial<T> result(n2);

    for (size_t k = 0; k < n2; ++k)
        result[k] = dft2[k].real();
#endif
    
    return result;
}

// shorthand mathematical operators
template <typename T>
polynomial<T> & polynomial<T>::operator += (const polynomial<T> & poly)
{
    size_t length = (m_degree <= poly.m_degree) ? m_degree : poly.m_degree;
    
    for (size_t n = 0; n < length; ++n)
        m_coeff[n] += poly.m_coeff[n];
    
    return *this;
}

template <typename T>
polynomial<T> & polynomial<T>::operator -= (const polynomial<T> & poly)
{
    size_t length = (m_degree <= poly.m_degree) ? m_degree : poly.m_degree;
    
    for (size_t n = 0; n < length; ++n)
        m_coeff[n] -= poly.m_coeff[n];
    
    return *this;
}

template <typename T>
polynomial<T> & polynomial<T>::operator *= (const polynomial<T> & poly)
{
    *this = (*this) * poly;
}

//---------------------------------------------------------------------------
// Entry point
int main(int argc, char ** argv)
{
    // are we testing using a genetic algorithm?
    bool ga_testing = false;
    
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[1],"-ga"))
            {
                ga_testing = true;
                break;
            }
        }
    }
    
    // generate random polynomials
    polynomial<double> poly1(TEST_SIZE);
    polynomial<double> poly2(TEST_SIZE);
    polynomial<double> poly3(TEST_SIZE * 2 - 1);
    
    for (int n = 0; n < TEST_SIZE; ++n)
    {
        poly1[n] = random_double();
        poly2[n] = random_double();
    }
    
    // get starting time    
    //struct timespec start, stop;
    //clock_gettime(CLOCK_REALTIME,&start);
    
    // what we're timing
    poly3 = poly1 * poly2;
    
    // get final time
    //clock_gettime(CLOCK_REALTIME,&stop);        
    double run_time = 0;
    //(stop.tv_sec - start.tv_sec) + (double)(stop.tv_nsec - start.tv_nsec) / 1000000000.0;

    // report runtime
    if (ga_testing)
        cout << run_time;
    else        
        cout << "\nfftbench (Std. C++) run time: " << run_time << "\n\n";
    
    cout.flush();
    
    return 0;
}
