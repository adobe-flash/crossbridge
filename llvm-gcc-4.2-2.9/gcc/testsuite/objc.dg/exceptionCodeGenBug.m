/* APPLE LOCAL file 4174405 */
/* { dg-options "-Os -mdynamic-no-pic -fobjc-exceptions -framework Foundation" } */
/* { dg-do run } */
// /usr/bin/gcc-4.0 -x objective-c -arch i386 -pipe -Wno-trigraphs -fpascal-strings -fasm-blocks -Os -Wreturn-type -Wunused-variable -fmessage-length=0 fvisibility=hidden -mdynamic-no-pic -framework Cocoa exceptionCodeGen.m

#import <Foundation/Foundation.h>

@interface MyObject : NSObject
{
}

@end

@implementation MyObject

- (void)subMethod:(id)sender;
{
  NSLog(@"subMethod");
}

- (void)doSomething;
{
  [self subMethod:nil];
}

@end

int main(int argc, char *argv[])
{
  NSAutoreleasePool *pool = [NSAutoreleasePool new];

  NSLog(@"Hello, world!");

  MyObject *object = [[MyObject alloc] init];

  NSException *exception = nil;

  SEL selector = @selector(doSomething);

  NSInvocation *invocation = nil;

  invocation = [NSInvocation invocationWithMethodSignature:[object methodSignatureForSelector:selector]];
  [invocation setTarget:object];
  [invocation setSelector:selector];
  [invocation retainArguments];
  
  NS_DURING
    [invocation invoke];
  NS_HANDLER
    exception = localException;
  NS_ENDHANDLER

  [exception raise];
  
  NSLog(@"Goodbye, world!");

  [pool release];

  exit(0);
  return 0;
}
