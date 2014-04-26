/* APPLE LOCAL file 4093475 */
/* Yet another stray infinite loop... */
/* { dg-do compile } */

@interface t
{
}
- (void)go;
@end
@implementation t
- (void)go
{
        }
} /* { dg-error "stray .\}. between Objective\\-C\\+\\+ methods" } */
@end

