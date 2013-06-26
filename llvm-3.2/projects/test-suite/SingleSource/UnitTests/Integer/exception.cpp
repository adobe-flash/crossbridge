//===--- exception.cpp --- Test Cases for Bit Accurate Types --------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is a test of exception handling with non-regular data
// type. The compiler should be able to differentiate int31, int7 etc.
//
//===----------------------------------------------------------------------===//


#include <stdio.h>

typedef int __attribute__ ((bitwidth(31))) int31;
typedef int __attribute__ ((bitwidth(1))) int1;
typedef int __attribute__ ((bitwidth(7))) int7;
typedef int __attribute__ ((bitwidth(17))) int17;

const int31 ex_num = 10;

double throw_test(double x, int31 y)
{
  if(y==0)
    throw ex_num;
  if(y==1)
    throw (int1)true;
  if(y==2)
    throw (int7)2;
  if(y == 3)
    throw (int17)3;
  return x;
}

int main()
{
  try{
    throw_test(10, 3);
  } catch(int31 i){
    printf("int31 branch\n");
  } catch(int1 b){
    printf("int1 branch\n");
  } catch(int7 c){
    printf("int7 branch\n");
  } catch(int17 s){
    printf("int17 branch\n");
  }

  return 0;
}
