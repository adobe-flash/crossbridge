/* APPLE LOCAL file radar 4660569 */
/* No warning here because accessor methods are INHERITED from NSButton */
/* APPLE LOCAL radar 4899595 */
/* { dg-options "-mmacosx-version-min=10.5" } */
/* AppKit not available on arm-darwin targets */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */
#include <AppKit/AppKit.h>

@interface NSButton (Properties)
@property (copy) NSString *title;
@end

@implementation NSButton (Properties)
@dynamic title;
@end
