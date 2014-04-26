/* { dg-options "-fpreprocessed" } */
/* { dg-compile } */

void foo () { 
  int a[] = {
# 3 "blah.h" 1 
    2,
    3,
    4,
    5,
    6,
    8,
    9,
10
# 30 "blah1.h" 1 
  };
}
