/* File : example.i */

/* This file has a few "typical" uses of C++ references. */

%module ExampleModule

%{
#include "example.h"
%}

%rename(cprint) print;

class CVector {
public:
    CVector(double x, double y, double z);
   ~CVector();
    char *print();
};

/* This helper function calls an overloaded operator */
%inline %{
CVector addv(CVector &a, CVector &b) {
  return a+b;
}
%}

/* Wrapper around an array of vectors class */

class VectorArray {
public:
  VectorArray(int maxsize);
  ~VectorArray();
  int size();
  
  /* This wrapper provides an alternative to the [] operator */
  %extend {
    CVector &get(int index) {
      return (*$self)[index];
    }
    void set(int index, CVector &a) {
      (*$self)[index] = a;
    }
  }
};




