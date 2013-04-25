/*
 * Contributed by Nicola Pero <nicola@brainstorm.co.uk>
 * Fri Feb  2 11:48:01 GMT 2001
 */

#include <objc/objc.h>
/* APPLE LOCAL radar 4894756 */
#include "Object2.h"
#include <objc/Protocol.h>

@protocol MyProtocol
+ (bycopy id<MyProtocol>) bycopyMethod;
@end

@interface MyObject : Object <MyProtocol> 
@end

@implementation MyObject
+ (bycopy id<MyProtocol>) bycopyMethod
{
  return [MyObject alloc];
}
@end

int main (void)
{
  MyObject *object;

  object = [MyObject bycopyMethod];

   exit (0);
}


