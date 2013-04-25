/* APPLE LOCAL file Radar 4475058 */
/* { dg-compile } */
/* { dg-options "-O" } */

struct   C1
{
};

template <class T> class T1
{
private: 
  T * _p;
public: 
  T * operator -> () { return _p; }
};

class C2 : public C1
{
};

class C3
{
public:
  C1 * _c3f1(C2 *k, C1 *v);
  C2 * c3f1(C2 *k, C2 *v) { return (C2 *)_c3f1(k, v); }
};

typedef T1<C3> RC3;
class C4
{
public:
  void c4f1( C4 * p);
protected:
  RC3 _pN;
};

C2 *foo (C2 **p)
{
  return *p;
}

class C5 : public C2
{
};

void C4::c4f1( C4 * p) 
{     
  C2 * k;   
  C5 * n; 

  while (  ( n = (static_cast<C5*>(foo (&k)))))
    _pN->c3f1( k, n);  
}   
 
