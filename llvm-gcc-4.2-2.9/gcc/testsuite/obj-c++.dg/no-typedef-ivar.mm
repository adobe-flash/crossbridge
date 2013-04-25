/* APPLE LOCAL file 4652027 */
/* Test that objective-c++ issues error in declaring typedef among ivar lists. */
/* { dg-do compile } */
/* APPLE LOCAL radar 4894756 */
#include "../objc/execute/Object2.h"

template <class T> class vector {};

@interface CompileError : Object
{
        typedef vector<int> Buffer;	/* { dg-error "typedef declaration among ivars" } */
}
@end
