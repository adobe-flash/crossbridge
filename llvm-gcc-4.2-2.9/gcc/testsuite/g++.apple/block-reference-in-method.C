/* APPLE LOCAL file radar 6169580 */
/* Test use of blocks in member functions. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 " { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Block.h>
#include <stdio.h>

int recovered = 0;


#ifdef __cplusplus

int constructors = 0;
int destructors = 0;

#define CONST const

class TestObject
{
public:
	TestObject(CONST TestObject& inObj);
	TestObject();
	~TestObject();
	
	TestObject& operator=(CONST TestObject& inObj);
        
        void test(void);

	int version() CONST { return _version; }
private:
	mutable int _version;
};

TestObject::TestObject(CONST TestObject& inObj)
	
{
        ++constructors;
        _version = inObj._version;
	printf("%p (%d) -- TestObject(const TestObject&) called", this, _version); 
}


TestObject::TestObject()
{
        _version = ++constructors;
	printf("%p (%d) -- TestObject() called\n", this, _version); 
}


TestObject::~TestObject()
{
	printf("%p -- ~TestObject() called\n", this);
        ++destructors;
}

#if 1
TestObject& TestObject::operator=(CONST TestObject& inObj)
{
	printf("%p -- operator= called", this);
        _version = inObj._version;
	return *this;
}
#endif

void TestObject::test(void)  {
    void (^b)(void) = ^{ recovered = _version; };
    b();
}

#endif


void testRoutine() {
#ifdef __cplusplus
    TestObject one;

    
    one.test();
#else
    recovered = 1;
#endif
}
    
    

int main(char *argc, char *argv[]) {
    testRoutine();
    if (recovered == 1) {
        printf("%s: success\n", argv[0]);
        return (0);
    }
    printf("%s: *** didn't recover byref block variable\n", argv[0]);
    return (1);
}
