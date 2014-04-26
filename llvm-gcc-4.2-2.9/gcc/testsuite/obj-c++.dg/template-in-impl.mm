/* APPLE LOCAL file radar 5976344 */
/* Test that class template can be properly parsed and handled inside
   an @implementation. */

/* { dg-do compile } */

template <class T> struct TemplateDeclarationIsFineHere {};

template <class T> struct Template2 {};

@interface Test @end

@implementation Test

- (void) test {
        Template2<int> youCanInstantiateTemplatesNoProblem();
}

template <class T> struct TemplateDeclarationIsNoGoodHere {};

- (TemplateDeclarationIsNoGoodHere<int>*) Meth { return 0; }


@end

