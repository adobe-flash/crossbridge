/* APPLE LOCAL file radar 6243400 */
/* { dg-do run { target *-*-darwin[1-2][0-9]* } } */
/* { dg-options "-fblocks" } */
/* { dg-skip-if "" { powerpc*-*-darwin* } { "-m64" } { "" } } */

#include <stdio.h>
extern "C" void abort(void);
int constructors = 0;
int destructors = 0;

class TestObject
{
public:
	TestObject(const TestObject& inObj);
	TestObject();
	~TestObject();
	
	TestObject& operator=(const TestObject& inObj);

	int version() const { return _version; }
private:
	mutable int _version;
};

TestObject::TestObject(const TestObject& inObj)
	
{
        ++constructors;
        _version = inObj._version;
	printf("%p (%d) -- TestObject(const TestObject&) called\n", this, _version); 
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


TestObject& TestObject::operator=(const TestObject& inObj)
{
	printf("%p -- operator= called\n", this);
        _version = inObj._version;
	return *this;
}

void testRoutine() {
    TestObject one;
    
    void (^b)(void) = ^{ printf("my const copy of one is %d\n", one.version()); };
}
    
    

int main(char *argc, char *argv[]) {
    testRoutine();
    if (constructors == 0) {
        printf("No copy constructors!!!\n");
        abort();
    }
    if (constructors != destructors) {
        printf("%d constructors but only %d destructors\n", constructors, destructors);
	abort();
    }
    printf("%s:success\n", argv[0]);
    return 0;
}
