/* APPLE LOCAL file Radar 4539933 */
/* Test guard variables for templated static data explicitly instantiated  */

// { dg-do run  }
// { dg-additional-sources "guard-1-aux.cpp" }

#include "guard-1.h"

int foo();

int result = -1;

int main()
{
    Test<int> theTest;
    foo();
    return result;
}
