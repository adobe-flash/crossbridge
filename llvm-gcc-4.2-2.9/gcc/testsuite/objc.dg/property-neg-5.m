/* APPLE LOCAL file 4724822 */
/* Attempt to access property of a forward decl. class. */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" { target powerpc*-*-darwin* i?86*-*-darwin* } } */

@class NSString;
void foo(NSString *param_0) {
        param_0.val; /* { dg-error "request for member \\'val\\' in something not a structure or union" } */
}

