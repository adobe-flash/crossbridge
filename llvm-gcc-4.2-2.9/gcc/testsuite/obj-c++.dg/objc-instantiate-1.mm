/* APPLE LOCAL file radar 4439126 */
/* Test to see if instantiation occurs before meta-data is generated. */
/* { dg-do run { target *-*-darwin* } } */
/* { dg-options "-framework Foundation" } */
#import <Foundation/Foundation.h>

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
