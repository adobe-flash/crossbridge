// APPLE LOCAL file
// Test case taken from SKTGridPanelController.m
// Sketch Example
//
/* Radar 5359827  */
/* { dg-do compile { target *-*-darwin* } } */
/* { dg-options "-O0 -g -c -save-temps -dA --no-warn" } */
/* { dg-skip-if "Unmatchable assembly" { mmix-*-* } { "*" } { "" } } */
/* APPLE LOCAL ARM AppKit not available on darwin-arm */
/* { dg-skip-if "" { arm*-*-darwin* } { "*" } { "" } } */
/* { dg-final { scan-assembler "__debug_pubtypes" } } */
/* { dg-final { scan-assembler "long\[ \t]+0x\[0-9a-f]+\[ \t]+\[#;]\[ \t]+Length of Public Type Names Info" } } */
/* { dg-final { scan-assembler "id\\\\0\"\[ \t]+\[#;]\[ \t]+external name" } } */


#import <AppKit/AppKit.h>

@implementation SKTGridPanelController

+ (id)sharedGridPanelController {
    static SKTGridPanelController *sharedGridPanelController = nil;

    if (!sharedGridPanelController) {
        sharedGridPanelController = [[SKTGridPanelController allocWithZone:NULL] init];
    }

    return sharedGridPanelController;
}

- (id)init {
    self = [self initWithWindowNibName:@"GridPanel"];
    if (self) {
        [self setWindowFrameAutosaveName:@"Grid"];
    }
    return self;
}

