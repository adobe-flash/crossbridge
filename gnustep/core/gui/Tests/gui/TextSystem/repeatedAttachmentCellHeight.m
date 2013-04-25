/*
copyright 2004 Alexander Malmberg <alexander@malmberg.org>

Check that the layour process doesn't get stuck if we try to make a cell fill
the entire height of a line frag.
*/

#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSString.h>
#include <AppKit/NSApplication.h>
#include <AppKit/NSAttributedString.h>
#include <AppKit/NSLayoutManager.h>
#include <AppKit/NSTextContainer.h>
#include <AppKit/NSTextStorage.h>

@interface MyCell : NSTextAttachmentCell
@end

@implementation MyCell

-(NSRect) cellFrameForTextContainer: (NSTextContainer *)textContainer
	proposedLineFragment: (NSRect)lineFrag
	glyphPosition: (NSPoint)p
	characterIndex: (unsigned int)charIndex
{
static int count;
	/* This should only be called once. */
	if (count)
		exit(1);
	count++;
	return NSMakeRect(0,-p.y,50,lineFrag.size.height);
}

@end

int main(int argc, char **argv)
{
	unichar chars[4]={'a','b','c',NSAttachmentCharacter};
	CREATE_AUTORELEASE_POOL(arp);
	NSTextStorage *text;
	NSLayoutManager *lm;
	NSTextContainer *tc;
	NSTextAttachment *ta;

	[NSApplication sharedApplication];

	text=[[NSTextStorage alloc] init];
	lm=[[NSLayoutManager alloc] init];
	tc=[[NSTextContainer alloc] initWithContainerSize: NSMakeSize(500,5000)];
	[lm addTextContainer: tc];
	[text addLayoutManager: lm];

	ta=[[NSTextAttachment alloc] init];
	[ta setAttachmentCell: [[MyCell alloc] init]];

	[text beginEditing];
	[text appendAttributedString:
		[[NSAttributedString alloc] initWithString:
			[NSString stringWithCharacters: chars length: 4]]];
	[text addAttribute: NSAttachmentAttributeName
		value: ta
		range: NSMakeRange(3,1)];
	[text endEditing];
	[lm usedRectForTextContainer: tc];

	DESTROY(arp);
	return 0;
}

