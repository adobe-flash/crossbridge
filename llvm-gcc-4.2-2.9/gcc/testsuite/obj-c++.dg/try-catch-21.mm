/* APPLE LOCAL file radar 4502186 */
/* Check that in the presence of @try/@catch, we can use variable argument 
   list functions in the block. Program must compile with no error/warning. */
/* { dg-options "-fobjc-abi-version=1" } */
/* { dg-do compile { target *-*-darwin* } } */

@interface untitled
-(int)countArgs:(id)fmt, ...;
@end

@implementation untitled

-(int)countArgs:(id)fmt, ...
{
    __builtin_va_list ap;

    @try
    {
    }
    @catch (id nse)
    {
    }
        __builtin_va_start(ap,fmt);
    __builtin_va_end(ap);
    return 0;
}
@end
