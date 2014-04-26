/* APPLE LOCAL file radar 6214617 - modified for radar 5847213 */
/* { dg-options "-mmacosx-version-min=10.5 -ObjC++" { target *-*-darwin* } } */
/* { dg-do run } */

#include <stdio.h>
#include <stdlib.h>

int constructors = 0;
int destructors = 0;

void * _NSConcreteStackBlock[32];

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


TestObject& TestObject::operator=(CONST TestObject& inObj)
{
	printf("%p -- operator= called\n", this);
        _version = inObj._version;
	return *this;
}

void hack(void *block) {
    /* APPLE LOCAL begin radar 6329245 */
    // check flags to see if constructor/destructor is available;
    struct myblock {
        void *isa;
        int flags;
	int reserved;
        void (*Block_invoke)(void *);
        struct Block_descriptor_1 {
        unsigned long int reserved;     // NULL
        unsigned long int Block_size;  // sizeof(struct Block_literal_1)

        // optional helper functions
        void (*copyhelper)(void *dst, void *src);
        void (*disposehelper)(void *src);
        } *descriptor;
        long space[32];
    } myversion, *mbp = (struct myblock *)block;
    /* APPLE LOCAL end radar 6329245 */
    printf("flags -> %x\n", mbp->flags);
    if (! ((1<<25) & mbp->flags)) {
        printf("no copy/dispose helper functions provided!\n");
        exit(1);
    }
    if (! ((1<<26) & mbp->flags)) {
        printf("no marking for ctor/dtors present!\n");
        exit(1);
    }
    printf("copyhelper -> %p\n", mbp->descriptor->copyhelper);
    // simulate copy
    mbp->descriptor->copyhelper(&myversion, mbp);
    if (constructors != 3) {
        printf("copy helper didn't do the constructor part\n");
        exit(1);
    }
    printf("disposehelper -> %p\n", mbp->descriptor->disposehelper);
    // simulate destroy
    mbp->descriptor->disposehelper(&myversion);
    if (destructors != 1) {
        printf("dispose helper didn't do the dispose\n");
        exit(1);
    }
}
void testRoutine() {
    TestObject one;
    
    void (^b)(void) = ^{ printf("my copy of one is %d\n", one.version()); };
    hack(b);
}

int main(char *argc, char *argv[]) {
    testRoutine();
    exit(0);
}
