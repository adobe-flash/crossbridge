/* APPLE LOCAL file 4557092 */
/* Test correct generation of cfstring tree in a function template. */
/* { dg-options "-framework Foundation" } */
/* { dg-do run { target *-*-darwin* } } */

#include <Foundation/Foundation.h>

template <typename T> class getter
{
public:
    NSString* foo ( T* obj )
    {
        return @"target2";
    }
};

int main (int argc, const char * argv[])
{

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    {

        getter<NSObject> g;

        NSLog ( @"String addr = [%x]", g.foo ( nil )  );
        NSLog ( @"String = [%@]", g.foo ( nil ) );

    }

    [pool release];
    return 0;
}

