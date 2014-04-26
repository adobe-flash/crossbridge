/* APPLE LOCAL file radar 4505126 */
/* Test lookup of properties in categories. */
/* Program should compile with no error or warning. */
/* { dg-do compile { target *-*-darwin* } } */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" } */
/* Cocoa not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */
#import <Cocoa/Cocoa.h>

@interface NSWindow (Properties)
@property(readonly) NSSize size;
@property(copy) NSString* title;
@end

@implementation NSWindow (Properties)
@dynamic title;

- (NSSize)size {
    return _frame.size;
}

@end

int main(int argc, char **argv) {
    NSAutoreleasePool *pool = [NSAutoreleasePool new];

    NSWindow *window = [[NSWindow new] autorelease];
    window.title = @"test1";
    NSLog(@"window.title = %@", window.title);
    NSSize size = window.size;

    [pool drain];
    return 0;
}

