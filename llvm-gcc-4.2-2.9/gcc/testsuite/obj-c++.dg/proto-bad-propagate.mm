/* APPLE LOCAL begin radar 4310884 */
/* Any usage of Foo<Bar> (where Foo is a *not yet defined* class and Bar is a protocol) 
   in a declaration results in inability to cast a Foo-derived classes to Foo (due to 
   warning) afterwards.
*/
/* { dg-options "-Wall -Werror" } */
/* { dg-do compile } */

#import <Foundation/Foundation.h>


@class AClass;
@protocol AProtocol;

/* This line somehow "bounds" AProtocol and AClass, if AClass is not
   yet defined, resulting in the buggy warning below. */
void bar(AClass<AProtocol> *a1);

/* If AClass declaration is placed *before* the above of
   AnotherProtocol, the warning disappears. */

@interface AClass : NSObject
{}
@end

@interface AnotherClass : AClass
{}
@end

@implementation AnotherClass

- (void)foo
{
    /*
      For the next line we get the warning:
      "class 'AnotherClass' does not implement the 'AProtocol' protocol"
    */
    AClass *f = self;

    f = f; // just to avoid warning due to -Wunused
}

@end
/* APPLE LOCAL end radar 4310884 */
