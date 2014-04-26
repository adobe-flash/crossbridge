/* APPLE LOCAL file radar 6169527 */
/* Test for proper use of copy constructors in setting up various block
   support code. */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-mmacosx-version-min=10.6 " { target *-*-darwin* } } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#import <Block.h>
#include <stdio.h>

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


TestObject& TestObject::operator=(CONST TestObject& inObj)
{
	printf("%p -- operator= called", this);
        _version = inObj._version;
	return *this;
}

void simpletest() {
    TestObject one;
    TestObject two;
    printf("one (%d) two (%d)\n", one.version(), two.version());
    one = two;
    printf(" after one = two, one (%d) two (%d)\n", one.version(), two.version());
}


void testRoutine() {
    TestObject one;
    
    void (^b)(void) = ^{ printf("my const copy of one is %d\n", one.version()); };
}
    
int main(char *argc, char *argv[]) {
    //simpletest();
    testRoutine();
    if (constructors != 0) {
        printf("%s: success\n", argv[0]);
        return 0;
    }
    printf("%s: *** didn't copy construct\n", argv[0]);
    return 1;
}
