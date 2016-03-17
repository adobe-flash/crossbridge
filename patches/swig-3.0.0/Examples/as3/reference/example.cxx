/* File : example.cxx */

/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER)
# define _CRT_SECURE_NO_DEPRECATE
#endif

#include "example.h"
#include <stdio.h>
#include <stdlib.h>

CVector operator+(const CVector &a, const CVector &b) {
  CVector r;
  r.x = a.x + b.x;
  r.y = a.y + b.y;
  r.z = a.z + b.z;
  return r;
}

char *CVector::print() {
  static char temp[512];
  sprintf(temp,"CVector %p (%g,%g,%g)", this, x,y,z);
  return temp;
}

VectorArray::VectorArray(int size) {
  items = new CVector[size];
  maxsize = size;
}

VectorArray::~VectorArray() {
  delete [] items;
}

CVector &VectorArray::operator[](int index) {
  if ((index < 0) || (index >= maxsize)) {
    printf("Panic! Array index out of bounds.\n");
    exit(1);
  }
  return items[index];
}

int VectorArray::size() {
  return maxsize;
}

