/* APPLE LOCAL file radar 4951615 */
/* Check that @protect ivar's offset variable does not have 'private extern' 
   visibility */
/* { dg-options "-mmacosx-version-min=10.5 -m64" } */
/* { dg-do compile { target powerpc*-*-darwin* i?86*-*-darwin* } } */

#import <Foundation/Foundation.h>

@interface MyCppClass : NSObject {
    id  _myCppInstanceVariable;
}
@end

@implementation MyCppClass
@end
/* { dg-final { scan-assembler-not "private_extern" } } */
