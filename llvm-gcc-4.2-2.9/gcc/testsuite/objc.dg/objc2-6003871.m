/* APPLE LOCAL file radar 6003871 */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-fnext-runtime -fobjc-gc" } */
/* { dg-do compile } */

#import "Cocoa/Cocoa.h"

@interface TEST : NSObject 
{
        
        int *filesEnt[3000000];
}
@end


@implementation TEST
@end

