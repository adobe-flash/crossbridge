// { dg-do assemble  }
// GROUPS passed delete
/*
  Bug Id: 
  PRMS Id: p0000710
  Bug is : overloading operator delete in class def not allowed
*/

/*
  In addition to this bug, the compiler permits overloading operator
  delete in the class definition.  This is verboten, and should be
  caught by a regression suite.  In other words, the following is also a
  bug that's not caught:
*/


#include <stdlib.h>

extern "C" 
{
   int printf(const char*, ...);
}



class B
{
 public:
   int x;
   virtual ~B() {}
   void operator delete(void*,size_t s)
  {
    /* APPLE LOCAL default to Wformat-security 5764921 */
      printf("B::delete() %d\n", (int)s);
   }
   void operator delete(void*){}
};

int main()
{
   B* p = new B;
   delete p;
   return 0;
}
