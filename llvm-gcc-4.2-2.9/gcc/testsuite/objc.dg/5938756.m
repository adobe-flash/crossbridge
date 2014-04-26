/* APPLE LOCAL file 5938756 */
/* This code caused an ICE */
/* { dg-do compile } */
/* { dg-options "-gdwarf-2 -Os" } */

@class NSException;

struct empty {
};

@interface PC {
}
@end

@implementation PC
- (id) fn {
  signed char isSet = 0;
  struct empty sb;
  if (isSet) {
    @try { }
    @catch (NSException *localException) { }
  }
}
@end 
