/* APPLE LOCAL file radar 5409313 */
/* { dg-do compile } */


typedef struct objc_selector *SEL;

SEL sel;

void foo ()
{
    sel = @selector(); /* { dg-error "method name missing in @selector" } */
}
