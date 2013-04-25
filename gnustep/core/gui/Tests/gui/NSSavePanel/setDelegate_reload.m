/*
copyright 2005 Alexander Malmberg <alexander@malmberg.org>

Test that the file lists in NSSavePanel are reloaded properly when the
delegate changes.
*/

#include "Testing.h"

/*#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSBundle.h>
#include <AppKit/NSApplication.h>
#include <AppKit/NSBrowser.h>
#include <AppKit/NSMatrix.h>
#include <AppKit/NSSavePanel.h>
#include <AppKit/NSWindow.h>*/
#include <AppKit/AppKit.h>

/* Ugly but automatable. :)  */
typedef struct
{
	@defs(NSSavePanel);
} NSSavePanel_ivars;

@interface Delegate : NSObject
@end

@implementation Delegate

static BOOL pressed;
static NSSavePanel *sp;

+(BOOL) panel: (NSSavePanel *)p
	shouldShowFilename: (NSString *)fname
{
//	printf("should show '%s'?\n",[fname cString]);
	if ([[fname lastPathComponent] isEqual: @"B"])
	{
		return NO;
	}
	return YES;
}

/*+(void) foo
{
	printf("did press button\n");
	pressed=YES;
	[sp validateVisibleColumns];
}*/

@end

int main(int argc, char **argv)
{
	NSAutoreleasePool *arp=[NSAutoreleasePool new];
	NSSavePanel *p;
	NSBrowser *b;
	NSMatrix *m;

	[NSApplication sharedApplication];

	sp=p=[NSSavePanel savePanel];
        [p setShowsHiddenFiles: NO];
	[p setDirectory: [[[[[NSBundle mainBundle] bundlePath]
		stringByDeletingLastPathComponent] stringByDeletingLastPathComponent]
		stringByAppendingPathComponent: @"dummy"]];

	b=((NSSavePanel_ivars *)p)->_browser;
	m=[b matrixInColumn: [b lastColumn]];
	pass([m numberOfRows] == 2
	     && [[[m cellAtRow: 0 column: 0] stringValue] isEqual: @"A"]
	     && [[[m cellAtRow: 1 column: 0] stringValue] isEqual: @"B"],
		"browser initially contains all files");

	[p setDelegate: [Delegate self]];
	b=((NSSavePanel_ivars *)p)->_browser;
	m=[b matrixInColumn: [b lastColumn]];
	pass([m numberOfRows] == 1
	     && [[[m cellAtRow: 0 column: 0] stringValue] isEqual: @"A"],
		"browser is reloaded after -setDelegate:");

	/* Not really a -setDelegate: issue, but the other methods involved are
	   documented as doing the wrong thing.  */
	[p setDelegate: nil];
	b=((NSSavePanel_ivars *)p)->_browser;
	m=[b matrixInColumn: [b lastColumn]];
        testHopeful = YES;
	pass([m numberOfRows] == 2
	     && [[[m cellAtRow: 0 column: 0] stringValue] isEqual: @"A"]
	     && [[[m cellAtRow: 1 column: 0] stringValue] isEqual: @"B"],
		"browser contains all files after resetting delegate");
        testHopeful = NO;

	[b scrollColumnsLeftBy: [b lastColumn]];
	[p setDelegate: [Delegate self]];
	b=((NSSavePanel_ivars *)p)->_browser;
	m=[b matrixInColumn: [b lastColumn]];
	pass([m numberOfRows] == 1
	     && [[[m cellAtRow: 0 column: 0] stringValue] isEqual: @"A"],
		"browser is reloaded after -setDelegate: (2)");

	[arp release];
	return 0;
}

