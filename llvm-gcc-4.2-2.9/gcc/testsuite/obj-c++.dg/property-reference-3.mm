/* APPLE LOCAL file radar 6029577 */
/* Test for correct generation of getters which return reference typed ivars. */
/* { dg-options "-framework Foundation -mmacosx-version-min=10.5" } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

struct MyStruct
{
	int x;
	int y;
	int z;
};

@interface MyClass : NSObject
{
	MyStruct _foo;
}

@property (assign, readwrite) const MyStruct& foo;

- (const MyStruct&) foo;
- (void) setFoo:(const MyStruct&)inFoo;
@end

@implementation MyClass

- (const MyStruct&) foo
{
	return _foo;
}

- (void) setFoo:(const MyStruct&)inFoo
{
	_foo = inFoo;
}
@end

int main()
{
	MyClass* myClass = [[MyClass alloc] init];
		
	MyStruct myStruct = (MyStruct){1, 2, 3};
	myClass.foo = myStruct;

	const MyStruct& currentMyStruct = myClass.foo;   
        if (currentMyStruct.x != 1 || currentMyStruct.y != 2 || currentMyStruct.z != 3)
	  abort();
	return 0;
}

