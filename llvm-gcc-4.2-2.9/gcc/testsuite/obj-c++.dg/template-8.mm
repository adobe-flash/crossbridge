/* APPLE LOCAL file radar 4439126 */
// Test that all pending instantiations have taken place before meta-data
// generation.
// Author: Fariborz Jahanian <fjahanian@apple.com>
/* { dg-options "-framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

class Base
{
public:
	Base() { }
	virtual ~Base() { }
	
	void destroy() { delete this; }
};

template<class T>
class Subclass : public T
{
public:
	Subclass() { }
	
	virtual ~Subclass()
	{
		[[NSNotificationCenter defaultCenter] removeObserver: nil];
	}
};

int main(int argc, const char * argv[])
{
    Subclass<Base>* theSubclass = new Subclass<Base>();

    theSubclass->destroy(); // THIS WILL NOT COMPILE
    return 0;
}
