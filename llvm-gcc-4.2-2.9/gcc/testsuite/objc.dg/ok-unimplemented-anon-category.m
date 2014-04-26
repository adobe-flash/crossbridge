/* APPLE LOCAL file radar 6370136  */
/* Fix regression caused by fix to radar 6017984 - Test must clean compile. */
/* { dg-options "-Werror" } */
/* { dg-do compile { target *-*-darwin* } } */

#import <Cocoa/Cocoa.h>

@interface Foo : NSView <NSTextViewDelegate> {}
@end

@interface FooTextView : NSTextView {}
@end

@protocol FooTextViewDelegate <NSTextViewDelegate>
- (void)bar:(FooTextView *)textView;
- (void)bas:(FooTextView *)textView;
- (void)bat:(FooTextView *)textView;
@end

@interface FooTextView() 
- (void)setDelegate:(id <FooTextViewDelegate>)delegate;
- (id <FooTextViewDelegate>)delegate;
@end

@interface Foo() <FooTextViewDelegate>
@end

@implementation Foo
- (void)bar:(FooTextView *)textView {}

- (void)bas:(FooTextView *)textView {}

- (void)bat:(FooTextView *)textView {}
@end

@implementation FooTextView
- (void)setDelegate:(id <FooTextViewDelegate>)delegate {
    [super setDelegate:delegate];
}

- (id <FooTextViewDelegate>)delegate {
    return (id <FooTextViewDelegate>)[super delegate];
}

- (void)jellyBean:(id)sender {
    [[self delegate] bas:self];
    [[self delegate] bat:self];
    [[self delegate] bar:self];
}

@end
