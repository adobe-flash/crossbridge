/*
copyright 2005 Alexander Malmberg <alexander@malmberg.org>
*/

#include "Testing.h"

#include <AppKit/NSApplication.h>
#include <AppKit/NSPopUpButton.h>

int main(int argc, char **argv)
{
	CREATE_AUTORELEASE_POOL(arp);
	NSPopUpButton *b;

	[NSApplication sharedApplication];

	b=[[NSPopUpButton alloc] init];

	[b addItemWithTitle: @"foo"];
	[b addItemWithTitle: @"bar"];

	pass([b indexOfSelectedItem] == 0,"first item is selected by default");

	DESTROY(arp);

	return 0;
}

