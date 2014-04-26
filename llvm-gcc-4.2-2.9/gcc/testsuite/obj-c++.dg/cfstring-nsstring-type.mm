/* APPLE LOCAL file radar 5982789 */
/* Type of a CFString literal is NSString. This test case
   should not issue any warning as "length" is a method 
   in NSString class.
*/
/* { dg-options "-Wall -Werror" } */
/* { dg-do compile } */

#include <Foundation/Foundation.h>

@interface MyTestClassA : NSObject
{
}
- (short) length;
@end

@implementation MyTestClassA
- (short) length;
{
    return 1;
}
@end


int main (int argc, const char * argv[])
{    
    NSUInteger testLen = [@"teststring" length];
    NSLog(@"teststring: %ld", (long)testLen);
        
    return 0;
}
