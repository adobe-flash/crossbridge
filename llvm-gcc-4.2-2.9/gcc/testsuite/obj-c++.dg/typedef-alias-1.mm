/* Typedefs of ObjC types should work without any bogus warnings. */
/* { dg-do compile } */

/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

typedef Object MyObject;

int main (int argc, const char * argv[])
{
    Object* a = nil;
    MyObject* b = a;
    Object* c = b;

    return 0;
}

