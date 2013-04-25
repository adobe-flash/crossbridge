/** <title>GSThemeDrawing</title>

   <abstract>The theme methods for drawing controls</abstract>

   Copyright (C) 2004-2010 Free Software Foundation, Inc.

   Author: Adam Fedor <fedor@gnu.org>
   Date: Jan 2004
   
   This file is part of the GNU Objective C User interface library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.
   If not, see <http://www.gnu.org/licenses/> or write to the 
   Free Software Foundation, 51 Franklin Street, Fifth Floor, 
   Boston, MA 02110-1301, USA.
*/

#import "GSThemePrivate.h"

#import "Foundation/NSUserDefaults.h"
#import "Foundation/NSIndexSet.h"

#import "AppKit/NSAttributedString.h"
#import "AppKit/NSBezierPath.h"
#import "AppKit/NSButtonCell.h"
#import "AppKit/NSBrowser.h"
#import "AppKit/NSCell.h"
#import "AppKit/NSColor.h"
#import "AppKit/NSColorList.h"
#import "AppKit/NSColorWell.h"
#import "AppKit/NSGraphics.h"
#import "AppKit/NSImage.h"
#import "AppKit/NSMenuView.h"
#import "AppKit/NSMenuItemCell.h"
#import "AppKit/NSParagraphStyle.h"
#import "AppKit/NSProgressIndicator.h"
#import "AppKit/NSScroller.h"
#import "AppKit/NSScrollView.h"
#import "AppKit/NSStringDrawing.h"
#import "AppKit/NSTableView.h"
#import "AppKit/NSTableColumn.h"
#import "AppKit/NSTableHeaderCell.h"
#import "AppKit/NSTableHeaderView.h"
#import "AppKit/NSView.h"
#import "AppKit/NSTabView.h"
#import "AppKit/NSTabViewItem.h"
#import "AppKit/PSOperators.h"
#import "AppKit/NSSliderCell.h"

#import "GNUstepGUI/GSToolbarView.h"
#import "GNUstepGUI/GSTitleView.h"

/* a border width of 5 gives a reasonable compromise between Cocoa metrics and looking good */
/* 7.0 gives us the NeXT Look (which is 8 pix wide including the shadow) */
#define COLOR_WELL_BORDER_WIDTH 7.0

@interface NSTableView (Private)
- (float *)_columnOrigins;
- (void) _willDisplayCell: (NSCell*)cell
	   forTableColumn: (NSTableColumn *)tb
		      row: (int)index;
@end

@interface NSCell (Private)
- (void) _setInEditing: (BOOL)flag;
@end

@implementation	GSTheme (Drawing)

- (void) drawButton: (NSRect)frame 
                 in: (NSCell*)cell 
               view: (NSView*)view 
              style: (int)style 
              state: (GSThemeControlState)state
{
  GSDrawTiles	*tiles = nil;
  NSColor	*color = nil;
  NSString	*name = [self nameForElement: cell];

  if (name == nil)
    {
      name = GSStringFromBezelStyle(style);
    }

  color = [self colorNamed: name state: state];
  if (color == nil)
    {
      if (state == GSThemeNormalState)
	{
          color = [NSColor controlBackgroundColor];
	}
      else if (state == GSThemeHighlightedState)
	{
          color = [NSColor selectedControlColor];
	}
      else if (state == GSThemeSelectedState)
	{
          color = [NSColor selectedControlColor];
	}
      else
    	{
          color = [NSColor controlBackgroundColor];
	}
    }

  tiles = [self tilesNamed: name state: state];
  if (tiles == nil)
    {
      tiles = [self tilesNamed: @"NSButton" state: state];
    }

  if (tiles == nil)
    {
      switch (style)
        {
	  case NSRoundRectBezelStyle:
	  case NSTexturedRoundedBezelStyle:
	  case NSRoundedBezelStyle:
	    [self drawRoundBezel: frame withColor: color];
	    break;
	  case NSTexturedSquareBezelStyle:
	    frame = NSInsetRect(frame, 0, 1);
	  case NSSmallSquareBezelStyle:
	  case NSRegularSquareBezelStyle:
	  case NSShadowlessSquareBezelStyle:
	    [color set];
	    NSRectFill(frame);
	    [[NSColor controlShadowColor] set];
	    NSFrameRectWithWidth(frame, 1);
	    break;
	  case NSThickSquareBezelStyle:
	    [color set];
	    NSRectFill(frame);
	    [[NSColor controlShadowColor] set];
	    NSFrameRectWithWidth(frame, 1.5);
	    break;
	  case NSThickerSquareBezelStyle:
	    [color set];
	    NSRectFill(frame);
	    [[NSColor controlShadowColor] set];
	    NSFrameRectWithWidth(frame, 2);
	    break;
	  case NSCircularBezelStyle:
	    frame = NSInsetRect(frame, 3, 3);
	    [self drawCircularBezel: frame withColor: color]; 
	    break;
	  case NSHelpButtonBezelStyle:
	    [self drawCircularBezel: frame withColor: color];
	    {
	      NSDictionary *attributes = [NSDictionary dictionaryWithObject: [NSFont controlContentFontOfSize: 0]
								     forKey: NSFontAttributeName];
	      NSAttributedString *questionMark = [[[NSAttributedString alloc]
						    initWithString: _(@"?")
							attributes: attributes] autorelease];

	      NSRect textRect;
	      textRect.size = [questionMark size];
	      textRect.origin.x = NSMidX(frame) - (textRect.size.width / 2);
	      textRect.origin.y = NSMidY(frame) - (textRect.size.height / 2);

	      [questionMark drawInRect: textRect];
	    }
	    break;
	  case NSDisclosureBezelStyle:
	  case NSRoundedDisclosureBezelStyle:
	  case NSRecessedBezelStyle:
	    // FIXME
	    break;
	  default:
	    [color set];
	    NSRectFill(frame);

	    if (state == GSThemeNormalState || state == GSThemeHighlightedState)
	      {
		[self drawButton: frame withClip: NSZeroRect];
	      }
	    else if (state == GSThemeSelectedState)
	      {
		[self drawGrayBezel: frame withClip: NSZeroRect];
	      }
	    else
	      {
		[self drawButton: frame withClip: NSZeroRect];
	      }
	}
    }
  else
    {
      /* Use tiles to draw button border with central part filled with color
       */
      [self fillRect: frame
	   withTiles: tiles
	  background: color];
    }
}

- (GSThemeMargins) buttonMarginsForCell: (NSCell*)cell
				  style: (int)style 
				  state: (GSThemeControlState)state
{
  GSDrawTiles	*tiles = nil;
  NSString	*name = [self nameForElement: cell];
  GSThemeMargins margins;

  if (name == nil)
    {
      name = GSStringFromBezelStyle(style);
    }

  tiles = [self tilesNamed: name state: state];
  if (tiles == nil)
    {
      tiles = [self tilesNamed: @"NSButton" state: state];
    } 

  if (tiles == nil)
    {
      switch (style)
        {
	  case NSRoundRectBezelStyle:
	  case NSTexturedRoundedBezelStyle:
	  case NSRoundedBezelStyle:
	    margins.left = 5; margins.top = 5; margins.right = 5; margins.bottom = 5;
	    return margins;
	  case NSTexturedSquareBezelStyle:
	    margins.left = 3; margins.top = 3; margins.right = 3; margins.bottom = 3;
	    return margins;
	  case NSSmallSquareBezelStyle:
	  case NSRegularSquareBezelStyle:
	  case NSShadowlessSquareBezelStyle:
	    margins.left = 2; margins.top = 2; margins.right = 2; margins.bottom = 2;
	    return margins;
	  case NSThickSquareBezelStyle:
	    margins.left = 3; margins.top = 3; margins.right = 3; margins.bottom = 3;
	    return margins;
	  case NSThickerSquareBezelStyle:
	    margins.left = 4; margins.top = 4; margins.right = 4; margins.bottom = 4;
	    return margins;
	  case NSCircularBezelStyle:
	    margins.left = 5; margins.top = 5; margins.right = 5; margins.bottom = 5;
	    return margins;
	  case NSHelpButtonBezelStyle:
	    margins.left = 2; margins.top = 2; margins.right = 2; margins.bottom = 2;
	    return margins;
	  case NSDisclosureBezelStyle:
	  case NSRoundedDisclosureBezelStyle:
	  case NSRecessedBezelStyle:
	    // FIXME
	    margins.left = 0; margins.top = 0; margins.right = 0; margins.bottom = 0;
	    return margins;
	  default:
	    margins.left = 2; margins.top = 2; margins.right = 3; margins.bottom = 3;
	    return margins;
	}
    }
  else
    {
      // FIXME: Move this code to a method in GSDrawTiles?
      // FIXME: Not correct, need to get the content area of the draw tiles
      margins.left = tiles->rects[TileCL].size.width;
      margins.top = tiles->rects[TileTM].size.height;
      margins.right = tiles->rects[TileCR].size.width;
      margins.bottom = tiles->rects[TileBM].size.height;
      return margins;
    }
}

- (void) drawFocusFrame: (NSRect) frame view: (NSView*) view
{
  NSDottedFrameRect(frame);
}

- (void) drawWindowBackground: (NSRect) frame view: (NSView*) view
{
  NSColor *c;

  c = [[view window] backgroundColor];
  [c set];
  NSRectFill (frame);
}

- (void) drawBorderType: (NSBorderType)aType 
                  frame: (NSRect)frame 
                   view: (NSView*)view
{
  NSString      *name = GSStringFromBorderType(aType);
  GSDrawTiles   *tiles = [self tilesNamed: name state: GSThemeNormalState];

  if (tiles == nil)
    {
      switch (aType)
	{
	  case NSLineBorder:
	    [[NSColor controlDarkShadowColor] set];
	    NSFrameRect(frame);
	    break;
	  case NSGrooveBorder:
	    [self drawGroove: frame withClip: NSZeroRect];
	    break;
	  case NSBezelBorder:
	    [self drawWhiteBezel: frame withClip: NSZeroRect];
	    break;
	  case NSNoBorder: 
	  default:
	    break;
	}
    }
  else
    {
      [self fillRect: frame
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (NSSize) sizeForBorderType: (NSBorderType)aType
{
  NSString      *name = GSStringFromBorderType(aType);
  GSDrawTiles   *tiles = [self tilesNamed: name state: GSThemeNormalState];

  if (tiles == nil)
    {
      // Returns the size of a border
      switch (aType)
	{
	  case NSLineBorder:
	    return NSMakeSize(1, 1);
	  case NSGrooveBorder:
	  case NSBezelBorder:
	    return NSMakeSize(2, 2);
	  case NSNoBorder: 
	  default:
	    return NSZeroSize;
	}
    }
  else
    {
      // FIXME: We assume the button's top and right padding are the same as
      // its bottom and left.
      return NSMakeSize(tiles->contentRect.origin.x,
                        tiles->contentRect.origin.y);
    }
}

- (void) drawBorderForImageFrameStyle: (NSImageFrameStyle)frameStyle
                                frame: (NSRect)frame 
                                 view: (NSView*)view
{
  switch (frameStyle)
    {
      case NSImageFrameNone:
        // do nothing
        break;
      case NSImageFramePhoto:
        [self drawFramePhoto: frame withClip: NSZeroRect];
        break;
      case NSImageFrameGrayBezel:
        [self drawGrayBezel: frame withClip: NSZeroRect];
        break;
      case NSImageFrameGroove:
        [self drawGroove: frame withClip: NSZeroRect];
        break;
      case NSImageFrameButton:
        [self drawButton: frame withClip: NSZeroRect];
        break;
    }
}

- (NSSize) sizeForImageFrameStyle: (NSImageFrameStyle)frameStyle
{
  // Get border size
  switch (frameStyle)
    {
      case NSImageFrameNone:
      default:
        return NSZeroSize;
      case NSImageFramePhoto:
        // FIXME
        return NSMakeSize(2, 2);
      case NSImageFrameGrayBezel:
      case NSImageFrameGroove:
      case NSImageFrameButton:
        return NSMakeSize(2, 2);
    }
}


/* NSScroller themeing.
 */
- (NSButtonCell*) cellForScrollerArrow: (NSScrollerArrow)arrow
			    horizontal: (BOOL)horizontal
{
  NSButtonCell	*cell;
  NSString	*name;
  
  cell = [NSButtonCell new];
  if (horizontal)
    {
      if (arrow == NSScrollerDecrementArrow)
	{
	  [cell setHighlightsBy:
	    NSChangeBackgroundCellMask | NSContentsCellMask];
	  [cell setImage: [NSImage imageNamed: @"common_ArrowLeft"]];
	  [cell setAlternateImage: [NSImage imageNamed: @"common_ArrowLeftH"]];
	  [cell setImagePosition: NSImageOnly];
          name = GSScrollerLeftArrow;
	}
      else
	{
	  [cell setHighlightsBy:
	    NSChangeBackgroundCellMask | NSContentsCellMask];
	  [cell setImage: [NSImage imageNamed: @"common_ArrowRight"]];
	  [cell setAlternateImage: [NSImage imageNamed: @"common_ArrowRightH"]];
	  [cell setImagePosition: NSImageOnly];
          name = GSScrollerRightArrow;
	}
    }
  else
    {
      if (arrow == NSScrollerDecrementArrow)
	{
	  [cell setHighlightsBy:
	    NSChangeBackgroundCellMask | NSContentsCellMask];
	  [cell setImage: [NSImage imageNamed: @"common_ArrowUp"]];
	  [cell setAlternateImage: [NSImage imageNamed: @"common_ArrowUpH"]];
	  [cell setImagePosition: NSImageOnly];
          name = GSScrollerUpArrow;
	}
      else
	{
	  [cell setHighlightsBy:
	    NSChangeBackgroundCellMask | NSContentsCellMask];
	  [cell setImage: [NSImage imageNamed: @"common_ArrowDown"]];
	  [cell setAlternateImage: [NSImage imageNamed: @"common_ArrowDownH"]];
	  [cell setImagePosition: NSImageOnly];
          name = GSScrollerDownArrow;
	}
    }
  [self setName: name forElement: cell temporary: YES];
  RELEASE(cell);
  return cell;
}

- (NSCell*) cellForScrollerKnob: (BOOL)horizontal
{
  NSButtonCell	*cell;

  cell = [NSButtonCell new];
  [cell setButtonType: NSMomentaryChangeButton];
  [cell setImagePosition: NSImageOnly];
  if (horizontal)
    {
      [self setName: GSScrollerHorizontalKnob forElement: cell temporary: YES];
      [cell setImage: [NSImage imageNamed: @"common_DimpleHoriz"]];
    }
  else
    {
      [self setName: GSScrollerVerticalKnob forElement: cell temporary: YES];
      [cell setImage: [NSImage imageNamed: @"common_Dimple"]];
  
    }
  RELEASE(cell);
  return cell;
}

- (NSCell*) cellForScrollerKnobSlot: (BOOL)horizontal
{
  GSDrawTiles   *tiles;
  NSButtonCell	*cell;
  NSColor	*color;
  NSString      *name;

  if (horizontal)
    {
      name = GSScrollerHorizontalSlot;
    }
  else
    {
      name = GSScrollerVerticalSlot;
    }

  tiles = [self tilesNamed: name state: GSThemeNormalState];
  color = [self colorNamed: name state: GSThemeNormalState];

  cell = [NSButtonCell new];
  [cell setBordered: (tiles != nil)];
  [cell setTitle: nil];

  [self setName: name forElement: cell temporary: YES];
 
  if (color == nil)
    {
      color = [NSColor scrollBarColor];
    }
  [cell setBackgroundColor: color];
  RELEASE(cell);
  return cell;
}

- (float) defaultScrollerWidth
{
  return 18.0;
}

- (NSColor *) toolbarBackgroundColor
{
  NSColor *color;

  color = [self colorNamed: @"toolbarBackgroundColor"
                state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor clearColor];
    }
  return color;
}

- (NSColor *) toolbarBorderColor
{
  NSColor *color;

  color = [self colorNamed: @"toolbarBorderColor"
                state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor darkGrayColor];
    }
  return color;
}

- (void) drawToolbarRect: (NSRect)aRect
                   frame: (NSRect)viewFrame
              borderMask: (unsigned int)borderMask
{
  // We draw the background
  [[self toolbarBackgroundColor] set];
  [NSBezierPath fillRect: aRect];
  
  // We draw the border
  [[self toolbarBorderColor] set];
  if (borderMask & GSToolbarViewBottomBorder)
    {
      [NSBezierPath strokeLineFromPoint: NSMakePoint(0, 0.5) 
                    toPoint: NSMakePoint(viewFrame.size.width, 0.5)];
    }
  if (borderMask & GSToolbarViewTopBorder)
    {
      [NSBezierPath strokeLineFromPoint: NSMakePoint(0, 
                                                     viewFrame.size.height - 0.5) 
                    toPoint: NSMakePoint(viewFrame.size.width, 
                                         viewFrame.size.height -  0.5)];
    }
  if (borderMask & GSToolbarViewLeftBorder)
    {
      [NSBezierPath strokeLineFromPoint: NSMakePoint(0.5, 0) 
                    toPoint: NSMakePoint(0.5, viewFrame.size.height)];
    }
  if (borderMask & GSToolbarViewRightBorder)
    {
      [NSBezierPath strokeLineFromPoint: NSMakePoint(viewFrame.size.width - 0.5,0)
                    toPoint: NSMakePoint(viewFrame.size.width - 0.5, 
                                         viewFrame.size.height)];
    }
}

- (BOOL) toolbarIsOpaque
{
  if ([[self toolbarBackgroundColor] alphaComponent] < 1.0)
    {
      return NO;
    }
  else
    {
      return YES;
    }
}

// NSStepperCell drawing
// Hard coded values for button sizes
#define STEPPER_WIDTH 15
#define STEPPER_HEIGHT 11

- (NSRect) stepperUpButtonRectWithFrame: (NSRect)frame
{
  NSRect upRect;

  upRect.size.width = STEPPER_WIDTH;
  upRect.size.height = STEPPER_HEIGHT;
  upRect.origin.x = NSMaxX(frame) - STEPPER_WIDTH - 1;
  upRect.origin.y = NSMinY(frame) + ((int)frame.size.height / 2) + 1;
  return upRect;
}

- (NSRect) stepperDownButtonRectWithFrame: (NSRect)frame
{
  NSRect downRect;

  downRect.size.width = STEPPER_WIDTH;
  downRect.size.height = STEPPER_HEIGHT;
  downRect.origin.x = NSMaxX(frame) - STEPPER_WIDTH - 1;
  downRect.origin.y = NSMinY(frame) + ((int)frame.size.height / 2) - STEPPER_HEIGHT + 1;
  return downRect;
}

- (void) drawStepperBorder: (NSRect)frame
{
  NSRectEdge up_sides[] = {NSMaxXEdge, NSMinYEdge};
  NSColor *black = [NSColor controlDarkShadowColor];
  NSColor *grays[] = {black, black}; 
  NSRect twoButtons;
  
  twoButtons.origin.x = NSMaxX(frame) - STEPPER_WIDTH - 1;
  twoButtons.origin.y = NSMinY(frame) + ((int)frame.size.height / 2) - STEPPER_HEIGHT;
  twoButtons.size.width = STEPPER_WIDTH + 1;
  twoButtons.size.height = 2 * STEPPER_HEIGHT + 1;
  
  NSDrawColorTiledRects(twoButtons, NSZeroRect,
                        up_sides, grays, 2);
}

- (NSRect) drawStepperLightButton: (NSRect)border : (NSRect)clip
{
/*
  NSRect highlightRect = NSInsetRect(border, 1., 1.);
  [[GSTheme theme] drawButton: border : clip];
  return highlightRect;
*/
  NSRectEdge up_sides[] = {NSMaxXEdge, NSMinYEdge, 
			   NSMinXEdge, NSMaxYEdge}; 
  NSRectEdge dn_sides[] = {NSMaxXEdge, NSMaxYEdge, 
			   NSMinXEdge, NSMinYEdge}; 
  // These names are role names not the actual colours
  NSColor *dark = [NSColor controlShadowColor];
  NSColor *white = [NSColor controlLightHighlightColor];
  NSColor *colors[] = {dark, dark, white, white};

  if ([[NSView focusView] isFlipped] == YES)
    {
      return NSDrawColorTiledRects(border, clip, dn_sides, colors, 4);
    }
  else
    {
      return NSDrawColorTiledRects(border, clip, up_sides, colors, 4);
    }
}

- (void) drawStepperUpButton: (NSRect)aRect
{
  NSRect unHighlightRect = [self drawStepperLightButton: aRect : NSZeroRect];
  [[NSColor controlBackgroundColor] set];
  NSRectFill(unHighlightRect);
      
  PSsetlinewidth(1.0);
  [[NSColor controlShadowColor] set];
  PSmoveto(NSMaxX(aRect) - 5, NSMinY(aRect) + 3);
  PSlineto(NSMaxX(aRect) - 8, NSMinY(aRect) + 9);
  PSstroke();
  [[NSColor controlDarkShadowColor] set];
  PSmoveto(NSMaxX(aRect) - 8, NSMinY(aRect) + 9);
  PSlineto(NSMaxX(aRect) - 11, NSMinY(aRect) + 4);
  PSstroke();
  [[NSColor controlLightHighlightColor] set];
  PSmoveto(NSMaxX(aRect) - 11, NSMinY(aRect) + 3);
  PSlineto(NSMaxX(aRect) - 5, NSMinY(aRect) + 3);
  PSstroke();
}

- (void) drawStepperHighlightUpButton: (NSRect)aRect
{
  NSRect highlightRect = [self drawStepperLightButton: aRect : NSZeroRect];
  [[NSColor selectedControlColor] set];
  NSRectFill(highlightRect);
  
  PSsetlinewidth(1.0);
  [[NSColor controlHighlightColor] set];
  PSmoveto(NSMaxX(aRect) - 5, NSMinY(aRect) + 3);
  PSlineto(NSMaxX(aRect) - 8, NSMinY(aRect) + 9);
  PSstroke();
  [[NSColor controlDarkShadowColor] set];
  PSmoveto(NSMaxX(aRect) - 8, NSMinY(aRect) + 9);
  PSlineto(NSMaxX(aRect) - 11, NSMinY(aRect) + 4);
  PSstroke();
  [[NSColor controlHighlightColor] set];
  PSmoveto(NSMaxX(aRect) - 11, NSMinY(aRect) + 3);
  PSlineto(NSMaxX(aRect) - 5, NSMinY(aRect) + 3);
  PSstroke();
}

- (void) drawStepperDownButton: (NSRect)aRect
{
  NSRect unHighlightRect = [self drawStepperLightButton: aRect : NSZeroRect];
  [[NSColor controlBackgroundColor] set];
  NSRectFill(unHighlightRect);

  PSsetlinewidth(1.0);
  [[NSColor controlShadowColor] set];
  PSmoveto(NSMinX(aRect) + 4, NSMaxY(aRect) - 3);
  PSlineto(NSMinX(aRect) + 7, NSMaxY(aRect) - 8);
  PSstroke();
  [[NSColor controlLightHighlightColor] set];
  PSmoveto(NSMinX(aRect) + 7, NSMaxY(aRect) - 8);
  PSlineto(NSMinX(aRect) + 10, NSMaxY(aRect) - 3);
  PSstroke();
  [[NSColor controlDarkShadowColor] set];
  PSmoveto(NSMinX(aRect) + 10, NSMaxY(aRect) - 2);
  PSlineto(NSMinX(aRect) + 4, NSMaxY(aRect) - 2);
  PSstroke();
}

- (void) drawStepperHighlightDownButton: (NSRect)aRect
{
  NSRect highlightRect = [self drawStepperLightButton: aRect : NSZeroRect];
  [[NSColor selectedControlColor] set];
  NSRectFill(highlightRect);
  
  PSsetlinewidth(1.0);
  [[NSColor controlHighlightColor] set];
  PSmoveto(NSMinX(aRect) + 4, NSMaxY(aRect) - 3);
  PSlineto(NSMinX(aRect) + 7, NSMaxY(aRect) - 8);
  PSstroke();
  [[NSColor controlHighlightColor] set];
  PSmoveto(NSMinX(aRect) + 7, NSMaxY(aRect) - 8);
  PSlineto(NSMinX(aRect) + 10, NSMaxY(aRect) - 3);
  PSstroke();
  [[NSColor controlDarkShadowColor] set];
  PSmoveto(NSMinX(aRect) + 10, NSMaxY(aRect) - 2);
  PSlineto(NSMinX(aRect) + 4, NSMaxY(aRect) - 2);
  PSstroke();
}

- (void) drawStepperCell: (NSCell*)cell
               withFrame: (NSRect)cellFrame
                  inView: (NSView*)controlView
             highlightUp: (BOOL)highlightUp
           highlightDown: (BOOL)highlightDown
{
  NSRect upRect;
  NSRect downRect;

  [self drawStepperBorder: cellFrame];

  upRect = [self stepperUpButtonRectWithFrame: cellFrame];
  downRect = [self stepperDownButtonRectWithFrame: cellFrame];
  
  if (highlightUp)
    [self drawStepperHighlightUpButton: upRect];
  else
    [self drawStepperUpButton: upRect];

  if (highlightDown)
    [self drawStepperHighlightDownButton: downRect];
  else
    [self drawStepperDownButton: downRect];
}

// NSSegmentedControl drawing methods

- (void) drawSegmentedControlSegment: (NSCell *)cell
                           withFrame: (NSRect)cellFrame
                              inView: (NSView *)controlView
                               style: (NSSegmentStyle)style  
                               state: (GSThemeControlState)state
                         roundedLeft: (BOOL)roundedLeft
                        roundedRight: (BOOL)roundedRight
{
  GSDrawTiles *tiles;
  NSString  *name = GSStringFromSegmentStyle(style);
  if (roundedLeft)
    {
      name = [name stringByAppendingString: @"RoundedLeft"];
    }
  if (roundedRight)
    {
      name = [name stringByAppendingString: @"RoundedRight"];
    }

  tiles = [self tilesNamed: name state: state];
 
  if (tiles == nil)
    {
      [self drawButton: cellFrame
                    in: cell
                  view: controlView
                 style: NSRegularSquareBezelStyle
                 state: state];
    }
  else
    {
      [self fillRect: cellFrame
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (NSColor *) menuBackgroundColor
{
  NSColor *color = [self colorNamed: @"menuBackgroundColor"
                              state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor windowBackgroundColor];
    }
  return color;
}

- (NSColor *) menuItemBackgroundColor
{
  NSColor *color = [self colorNamed: @"menuItemBackgroundColor"
                              state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor controlBackgroundColor];
    }
  return color;
}

- (NSColor *) menuBorderColor
{
  NSColor *color = [self colorNamed: @"menuBorderColor"
                              state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor darkGrayColor];
    }
  return color;
}

- (NSColor *) menuBarBackgroundColor
{
  NSColor *color = [self colorNamed: @"menuBarBackgroundColor"
                              state: GSThemeNormalState];
  if (color == nil)
    {
      color = [self menuBackgroundColor];
    }
  return color;
}

- (NSColor *) menuBarBorderColor
{
  NSColor *color = [self colorNamed: @"menuBarBorderColor"
                              state: GSThemeNormalState];
  if (color == nil)
    {
      color = [self menuBorderColor];
    }
  return color;
}

- (NSColor *) menuBorderColorForEdge: (NSRectEdge)edge isHorizontal: (BOOL)horizontal
{
  if (horizontal && edge == NSMinYEdge)
    {
      return [self menuBorderColor];
    }
  else if (edge == NSMinXEdge || edge == NSMaxYEdge)
    {
      // Draw the dark gray upper left lines.
      return [self menuBorderColor];
    }
  return nil;
}

- (void) drawBackgroundForMenuView: (NSMenuView*)menuView
                         withFrame: (NSRect)bounds
                         dirtyRect: (NSRect)dirtyRect
                        horizontal: (BOOL)horizontal 
{
  NSString  *name = horizontal ? GSMenuHorizontalBackground : 
    GSMenuVerticalBackground;
  GSDrawTiles *tiles = [self tilesNamed: name state: GSThemeNormalState];
 
  if (tiles == nil)
    {
      NSRectEdge sides[4] = { NSMinXEdge, NSMaxYEdge, NSMaxXEdge, NSMinYEdge }; 
      NSColor *colors[] = {[self menuBorderColorForEdge: NSMinXEdge isHorizontal: horizontal], 
                           [self menuBorderColorForEdge: NSMaxYEdge isHorizontal: horizontal], 
                           [self menuBorderColorForEdge: NSMaxXEdge isHorizontal: horizontal],
                           [self menuBorderColorForEdge: NSMinYEdge isHorizontal: horizontal]};

      [[self menuBackgroundColor] set];
      NSRectFill(NSIntersectionRect(bounds, dirtyRect));
      NSDrawColorTiledRects(bounds, dirtyRect, sides, colors, 4);
    }
  else
    {
      [self fillRect: bounds
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (BOOL) drawsBorderForMenuItemCell: (NSMenuItemCell *)cell 
                              state: (GSThemeControlState)state
                       isHorizontal: (BOOL)horizontal
{
  return [cell isBordered];
}

- (void) drawBorderAndBackgroundForMenuItemCell: (NSMenuItemCell *)cell
                                      withFrame: (NSRect)cellFrame
                                         inView: (NSView *)controlView
                                          state: (GSThemeControlState)state
                                   isHorizontal: (BOOL)isHorizontal
{
  NSString  *name = isHorizontal ? GSMenuHorizontalItem :
    GSMenuVerticalItem;
  GSDrawTiles *tiles = [self tilesNamed: name state: state];
 
  if (tiles == nil)
    {
      NSColor	*backgroundColor = [cell backgroundColor];

      if (isHorizontal)
	{
	  cellFrame = [cell drawingRectForBounds: cellFrame];
	  [backgroundColor set];
	  NSRectFill(cellFrame);
	  return;
	}

      // Set cell's background color
      [backgroundColor set];
      NSRectFill(cellFrame);

      if (![self drawsBorderForMenuItemCell: cell 
                                      state: state 
                               isHorizontal: isHorizontal])
        {
          return;
        }

      if (state == GSThemeSelectedState)
	{
          [self drawGrayBezel: cellFrame withClip: NSZeroRect];
        }
      else
        {
          [self drawButton: cellFrame withClip: NSZeroRect];
        }
    }
  else
    {
      [self fillRect: cellFrame
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (NSColor *) menuSeparatorColor
{
  NSColor *color = [self colorNamed: @"menuSeparatorColor"
                              state: GSThemeNormalState];
  NSInterfaceStyle style = NSInterfaceStyleForKey(@"NSMenuInterfaceStyle", nil);

  // TODO: Remove the style check... Windows theming should be in a subclass 
  // probably
  if (color == nil && style == NSWindows95InterfaceStyle)
    {
      color = [NSColor blackColor];
    }
  return color;
}

- (CGFloat) menuSeparatorInset
{
  return 3.0;
}

- (void) drawSeparatorItemForMenuItemCell: (NSMenuItemCell *)cell
                                withFrame: (NSRect)cellFrame
                                   inView: (NSView *)controlView
                             isHorizontal: (BOOL)isHorizontal
{
  GSDrawTiles *tiles = [self tilesNamed: GSMenuSeparatorItem state: GSThemeNormalState];
 
  if (tiles == nil)
    {
      NSBezierPath *path = [NSBezierPath bezierPath];
      CGFloat inset = [self menuSeparatorInset];
      NSPoint start = NSMakePoint(inset, cellFrame.size.height / 2 + 
				         cellFrame.origin.y + 0.5);
      NSPoint end = NSMakePoint(cellFrame.size.width - inset, 
	                        cellFrame.size.height / 2 + cellFrame.origin.y + 0.5);

      [[self menuSeparatorColor] set];

      [path setLineWidth: 0.0];
      [path moveToPoint: start];
      [path lineToPoint: end];

      [path stroke];
    }
  else
    {
      [self fillRect: cellFrame
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (void) drawTitleForMenuItemCell: (NSMenuItemCell *)cell
                        withFrame: (NSRect)cellFrame
                           inView: (NSView *)controlView
                            state: (GSThemeControlState)state
                     isHorizontal: (BOOL)isHorizontal
{
  [cell _drawText: [[cell menuItem] title]
          inFrame: [cell titleRectForBounds: cellFrame]];
}

- (Class) titleViewClassForMenuView: (NSMenuView *)aMenuView
{
  return [GSTitleView class];
}

// NSColorWell drawing method
- (NSRect) drawColorWellBorder: (NSColorWell*)well
                    withBounds: (NSRect)bounds
                      withClip: (NSRect)clipRect
{
  NSRect aRect = bounds;

  if ([well isBordered])
    {
      GSThemeControlState state;
      GSDrawTiles *tiles;

      if ([[well cell] isHighlighted] || [well isActive])
	{
          state = GSThemeHighlightedState;
	}
      else
	{
          state = GSThemeNormalState;
	}

      tiles = [self tilesNamed: GSColorWell state: state];
      if (tiles == nil)
        {
	  /*
	   * Draw border.
	   */
	  [self drawButton: aRect withClip: clipRect];

	  /*
	   * Fill in control color.
	   */
	  if (state == GSThemeHighlightedState)
	    {
	      [[NSColor selectedControlColor] set];
	    }
	  else
	    {
	      [[NSColor controlColor] set];
	    }
	  aRect = NSInsetRect(aRect, 2.0, 2.0);
	  NSRectFill(NSIntersectionRect(aRect, clipRect));
        }
      else
        {
          aRect = [self fillRect: aRect
                       withTiles: tiles
                      background: [NSColor clearColor]];
        }

      /*
       * Set an inset rect for the color area
       */
      aRect = NSInsetRect(bounds, COLOR_WELL_BORDER_WIDTH, COLOR_WELL_BORDER_WIDTH);
    }

  /*
   * OpenStep 4.2 behavior is to omit the inner border for
   * non-enabled NSColorWell objects.
   */
  if ([well isEnabled])
    {
      /*
       * Draw inner frame.
       */
      [self drawGrayBezel: aRect withClip: clipRect];
      aRect = NSInsetRect(aRect, 2.0, 2.0);
    }

  return aRect;
}

// progress indicator drawing methods
static NSColor *fillColour = nil;
#define MaxCount 10
static int indeterminateMaxCount = MaxCount;
static int spinningMaxCount = MaxCount;
static NSColor *indeterminateColors[MaxCount];
static NSImage *spinningImages[MaxCount];

- (void) initProgressIndicatorDrawing
{
  int i;
  
  // FIXME: Should come from defaults and should be reset when defaults change
  // FIXME: Should probably get the color from the color extension list (see NSToolbar)
  fillColour = RETAIN([NSColor controlShadowColor]);

  // Load images for indeterminate style
  for (i = 0; i < MaxCount; i++)
    {
      NSString *imgName = [NSString stringWithFormat: @"common_ProgressIndeterminate_%d", i + 1];
      NSImage *image = [NSImage imageNamed: imgName];
      
      if (image == nil)
        {
          indeterminateMaxCount = i;
          break;
        }
          indeterminateColors[i] = RETAIN([NSColor colorWithPatternImage: image]);
    }
  
  // Load images for spinning style
  for (i = 0; i < MaxCount; i++)
    {
      NSString *imgName = [NSString stringWithFormat: @"common_ProgressSpinning_%d", i + 1];
      NSImage *image = [NSImage imageNamed: imgName];
      
      if (image == nil)
        {
          spinningMaxCount = i;
          break;
        }
      spinningImages[i] = RETAIN(image); 
    }
}

- (void) drawProgressIndicator: (NSProgressIndicator*)progress
                    withBounds: (NSRect)bounds
                      withClip: (NSRect)rect
                       atCount: (int)count
                      forValue: (double)val
{
   NSRect r;

   if (fillColour == nil)
     {
       [self initProgressIndicatorDrawing];
     }

   // Draw the Bezel
   if ([progress isBezeled])
     {
       // Calc the inside rect to be drawn
       r = [self drawProgressIndicatorBezel: bounds withClip: rect];
     }
   else
     {
       r = bounds;
     }

   if ([progress style] == NSProgressIndicatorSpinningStyle)
     {
       NSRect imgBox = {{0,0}, {0,0}};

       if (spinningMaxCount != 0)
	 {
	   count = count % spinningMaxCount;
	   imgBox.size = [spinningImages[count] size];
	   [spinningImages[count] drawInRect: r 
				    fromRect: imgBox 
				   operation: NSCompositeSourceOver
				    fraction: 1.0];
	 }
     }
   else
     {
       if ([progress isIndeterminate])
         {
	   if (indeterminateMaxCount != 0)
	     {
	       count = count % indeterminateMaxCount;
	       [indeterminateColors[count] set];
	       NSRectFill(r);
	     }
         }
       else
         {
           // Draw determinate 
           if ([progress isVertical])
             {
               float height = NSHeight(r) * val;
               
               if ([progress isFlipped])
                 {
                   // Compensate for the flip
                   r.origin.y += NSHeight(r) - height;
                 }
               r.size.height = height;
             }
           else
             {
               r.size.width = NSWidth(r) * val;
             }
           r = NSIntersectionRect(r, rect);
           if (!NSIsEmptyRect(r))
             {
               [self drawProgressIndicatorBarDeterminate: (NSRect)r];
             }
         }
     }
}

- (NSRect) drawProgressIndicatorBezel: (NSRect)bounds withClip: (NSRect) rect
{
  return [self drawGrayBezel: bounds withClip: rect];
}

- (void) drawProgressIndicatorBarDeterminate: (NSRect)bounds
{
  GSDrawTiles *tiles = [self tilesNamed: GSProgressIndicatorBarDeterminate
                                  state: GSThemeNormalState];

  if (tiles == nil)
    {
      [fillColour set];
      NSRectFill(bounds);
    }
  else
    {
      [self fillRect: bounds
           withTiles: tiles
          background: fillColour];
    }
}

// Table drawing methods
- (void) drawTableCornerView: (NSView*)cornerView
                   withClip: (NSRect)aRect
{
  NSRect divide;
  NSRect rect;
  GSDrawTiles *tiles = [self tilesNamed: GSTableCorner state: GSThemeNormalState];

  if ([cornerView isFlipped])
    {
      NSDivideRect(aRect, &divide, &rect, 1.0, NSMaxYEdge);
    }
  else
    {
      NSDivideRect(aRect, &divide, &rect, 1.0, NSMinYEdge);
    }

  if (tiles == nil)
    { 
      [[NSColor blackColor] set];
      NSRectFill(divide);
      rect = [self drawDarkButton: rect withClip: aRect];
      [[NSColor controlShadowColor] set];
      NSRectFill(rect);
    }
  else
    {
       [self fillRect: aRect
            withTiles: tiles
           background: [NSColor clearColor]];
    }
}

- (void) drawTableHeaderCell: (NSTableHeaderCell *)cell
                   withFrame: (NSRect)cellFrame
                      inView: (NSView *)controlView
                       state: (GSThemeControlState)state
{
  GSDrawTiles *tiles = [self tilesNamed: GSTableHeader state: state];

  if (tiles == nil)
    {
      NSRect rect;

      // Leave a 1pt thick horizontal line underneath the header
      if (![controlView isFlipped])
        {
          cellFrame.origin.y++;
        }
      cellFrame.size.height--;

      if (state == GSThemeHighlightedState)
        {
          rect = [self drawButton: cellFrame withClip: cellFrame];
          [[NSColor controlColor] set];
          NSRectFill(rect);        
        }
      else
        {
          rect = [self drawDarkButton: cellFrame withClip: cellFrame];
          [[NSColor controlShadowColor] set];
          NSRectFill(rect);
        }
    }
  else
    {
      [self fillRect: cellFrame
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}


// Window decoration drawing methods
/* These include the black border. */
#define TITLE_HEIGHT 23.0
#define RESIZE_HEIGHT 9.0

- (float) titlebarHeight
{
  return TITLE_HEIGHT;
}

- (float) resizebarHeight
{
  return RESIZE_HEIGHT;
}

static NSDictionary *titleTextAttributes[3] = {nil, nil, nil};

- (void) drawTitleBarRect: (NSRect)titleBarRect 
             forStyleMask: (unsigned int)styleMask
                    state: (int)inputState 
                 andTitle: (NSString*)title
{
  static const NSRectEdge edges[4] = {NSMinXEdge, NSMaxYEdge,
				    NSMaxXEdge, NSMinYEdge};
  float grays[3][4] =
    {{NSLightGray, NSLightGray, NSDarkGray, NSDarkGray},
    {NSWhite, NSWhite, NSDarkGray, NSDarkGray},
    {NSLightGray, NSLightGray, NSBlack, NSBlack}};
  NSRect workRect;
  GSDrawTiles *tiles = nil;

  if (!titleTextAttributes[0])
    {
      NSMutableParagraphStyle *p;
      NSColor *keyColor, *normalColor, *mainColor;

      p = [[NSParagraphStyle defaultParagraphStyle] mutableCopy];
      [p setLineBreakMode: NSLineBreakByClipping];

      // FIXME: refine color names based on style mask
      // (HUD or textured or regular window)

      keyColor = [self colorNamed: @"keyWindowFrameTextColor"
                            state: GSThemeNormalState];
      if (nil == keyColor)
        {
          keyColor = [NSColor windowFrameTextColor];
        }

      normalColor = [self colorNamed: @"normalWindowFrameTextColor"
                               state: GSThemeNormalState];
      if (nil == normalColor)
        {
          normalColor = [NSColor blackColor];
        }
 
      mainColor = [self colorNamed: @"mainWindowFrameTextColor"
                             state: GSThemeNormalState];
      if (nil == mainColor)
        {
          mainColor = [NSColor windowFrameTextColor];
        }
 
      titleTextAttributes[0] = [[NSMutableDictionary alloc]
	initWithObjectsAndKeys:
	  [NSFont titleBarFontOfSize: 0], NSFontAttributeName,
	  keyColor, NSForegroundColorAttributeName,
	  p, NSParagraphStyleAttributeName,
	  nil];

      titleTextAttributes[1] = [[NSMutableDictionary alloc]
	initWithObjectsAndKeys:
	  [NSFont titleBarFontOfSize: 0], NSFontAttributeName,
	  normalColor, NSForegroundColorAttributeName,
	  p, NSParagraphStyleAttributeName,
	  nil];

      titleTextAttributes[2] = [[NSMutableDictionary alloc]
	initWithObjectsAndKeys:
	  [NSFont titleBarFontOfSize: 0], NSFontAttributeName,
	  mainColor, NSForegroundColorAttributeName,
	  p, NSParagraphStyleAttributeName,
	  nil];

      RELEASE(p);
    }

  tiles = [self tilesNamed: @"GSWindowTitleBar" state: GSThemeNormalState];
  if (tiles == nil)
    {
      /*
      Draw the black border towards the rest of the window. (The outer black
      border is drawn in -drawRect: since it might be drawn even if we don't have
      a title bar.
      */
      NSColor *borderColor = [self colorNamed: @"windowBorderColor"
                                        state: GSThemeNormalState];
      if (nil == borderColor)
        {
          borderColor = [NSColor blackColor];
        }
      [borderColor set];
 
      PSmoveto(0, NSMinY(titleBarRect) + 0.5);
      PSrlineto(titleBarRect.size.width, 0);
      PSstroke();

      /*
      Draw the button-like border.
      */
      workRect = titleBarRect;
      workRect.origin.x += 1;
      workRect.origin.y += 1;
      workRect.size.width -= 2;
      workRect.size.height -= 2;

      workRect = NSDrawTiledRects(workRect, workRect, edges, grays[inputState], 4);
     
      /*
      Draw the background.
      */
      switch (inputState) 
	{
	default:
	case 0:
	  [[NSColor windowFrameColor] set];
	  break;
	case 1:
	  [[NSColor lightGrayColor] set];
	  break;
	case 2:
	  [[NSColor darkGrayColor] set];
	  break;
	}
      NSRectFill(workRect);
    }
  else
    {
      [self fillRect: titleBarRect
          withTiles: tiles
         background: [NSColor windowFrameColor]];
      workRect = titleBarRect;
    }
  /* Draw the title. */
  if (styleMask & NSTitledWindowMask)
    {
      NSSize titleSize;
    
      if (styleMask & NSMiniaturizableWindowMask)
	{
	  workRect.origin.x += 17;
	  workRect.size.width -= 17;
	}
      if (styleMask & NSClosableWindowMask)
	{
	  workRect.size.width -= 17;
	}
  
      titleSize = [title sizeWithAttributes: titleTextAttributes[inputState]];
      if (titleSize.width <= workRect.size.width)
	workRect.origin.x = NSMidX(workRect) - titleSize.width / 2;
      workRect.origin.y = NSMidY(workRect) - titleSize.height / 2;
      workRect.size.height = titleSize.height;
      [title drawInRect: workRect
	 withAttributes: titleTextAttributes[inputState]];
    }
}

// FIXME: Would be good if this took the window as a param
- (void) drawResizeBarRect: (NSRect)resizeBarRect
{
  GSDrawTiles *tiles;
  tiles = [self tilesNamed: @"GSWindowResizeBar" state: GSThemeNormalState];
  if (tiles == nil)
    {
      [[NSColor lightGrayColor] set];
      PSrectfill(1.0, 1.0, resizeBarRect.size.width - 2.0, RESIZE_HEIGHT - 3.0);

      PSsetlinewidth(1.0);

      [[NSColor blackColor] set];
      PSmoveto(0.0, 0.5);
      PSlineto(resizeBarRect.size.width, 0.5);
      PSstroke();

      [[NSColor darkGrayColor] set];
      PSmoveto(1.0, RESIZE_HEIGHT - 0.5);
      PSlineto(resizeBarRect.size.width - 1.0, RESIZE_HEIGHT - 0.5);
      PSstroke();

      [[NSColor whiteColor] set];
      PSmoveto(1.0, RESIZE_HEIGHT - 1.5);
      PSlineto(resizeBarRect.size.width - 1.0, RESIZE_HEIGHT - 1.5);
      PSstroke();


      /* Only draw the notches if there's enough space. */
      if (resizeBarRect.size.width < 30 * 2)
	return;

      [[NSColor darkGrayColor] set];
      PSmoveto(27.5, 1.0);
      PSlineto(27.5, RESIZE_HEIGHT - 2.0);
      PSmoveto(resizeBarRect.size.width - 28.5, 1.0);
      PSlineto(resizeBarRect.size.width - 28.5, RESIZE_HEIGHT - 2.0);
      PSstroke();

      [[NSColor whiteColor] set];
      PSmoveto(28.5, 1.0);
      PSlineto(28.5, RESIZE_HEIGHT - 2.0);
      PSmoveto(resizeBarRect.size.width - 27.5, 1.0);
      PSlineto(resizeBarRect.size.width - 27.5, RESIZE_HEIGHT - 2.0);
      PSstroke();
    }
  else
    {
      [self fillRect: resizeBarRect
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (void) drawWindowBorder: (NSRect)rect 
                withFrame: (NSRect)frame 
             forStyleMask: (unsigned int)styleMask
                    state: (int)inputState 
                 andTitle: (NSString*)title
{
  if (styleMask & (NSTitledWindowMask | NSClosableWindowMask 
                   | NSMiniaturizableWindowMask))
    {
      NSRect titleBarRect;

      titleBarRect = NSMakeRect(0.0, frame.size.height - TITLE_HEIGHT,
                                frame.size.width, TITLE_HEIGHT);
      if (NSIntersectsRect(rect, titleBarRect))
        [self drawTitleBarRect: titleBarRect 
              forStyleMask: styleMask
              state: inputState 
              andTitle: title];
    }

  if (styleMask & NSResizableWindowMask)
    {
      NSRect resizeBarRect;

      resizeBarRect = NSMakeRect(0.0, 0.0, frame.size.width, RESIZE_HEIGHT);
      if (NSIntersectsRect(rect, resizeBarRect))
        [self drawResizeBarRect: resizeBarRect];
    }

  if (styleMask & (NSTitledWindowMask | NSClosableWindowMask 
                   | NSMiniaturizableWindowMask | NSResizableWindowMask))
    {
      NSColor *borderColor = [self colorNamed: @"windowBorderColor"
                                        state: GSThemeNormalState];
      if (nil == borderColor)
        {
          borderColor = [NSColor blackColor];
        }
      [borderColor set];
      PSsetlinewidth(1.0);
      if (NSMinX(rect) < 1.0)
	{
	  PSmoveto(0.5, 0.0);
	  PSlineto(0.5, frame.size.height);
	  PSstroke();
	}
      if (NSMaxX(rect) > frame.size.width - 1.0)
	{
	  PSmoveto(frame.size.width - 0.5, 0.0);
	  PSlineto(frame.size.width - 0.5, frame.size.height);
	  PSstroke();
	}
      if (NSMaxY(rect) > frame.size.height - 1.0)
	{
	  PSmoveto(0.0, frame.size.height - 0.5);
	  PSlineto(frame.size.width, frame.size.height - 0.5);
	  PSstroke();
	}
      if (NSMinY(rect) < 1.0)
	{
	  PSmoveto(0.0, 0.5);
	  PSlineto(frame.size.width, 0.5);
	  PSstroke();
	}
    }
}


- (void) drawBrowserHeaderCell: (NSTableHeaderCell*)cell
	 	     withFrame: (NSRect)rect
			inView: (NSView*)view;
{
  GSDrawTiles *tiles;
  tiles = [self tilesNamed: GSBrowserHeader state: GSThemeNormalState];
  if (tiles == nil)
   {
     [self drawGrayBezel: rect withClip: NSZeroRect];
     [cell _drawBackgroundWithFrame: rect inView: view];
   }
  else
    {
      [self fillRect: rect
           withTiles: tiles
          background: [NSColor clearColor]];
    }
}

- (NSRect) browserHeaderDrawingRectForCell: (NSTableHeaderCell*)cell
				 withFrame: (NSRect)rect
{
  GSDrawTiles *tiles;
  tiles = [self tilesNamed: GSBrowserHeader state: GSThemeNormalState];
  if (tiles == nil)
    {
      return NSInsetRect(rect, 2, 2);
    }
  else
    {
      // FIXME: We assume the button's top and right padding are the same as
      // its bottom and left.
      return NSInsetRect(rect,
			 tiles->contentRect.origin.x,
			 tiles->contentRect.origin.y);
    }
}

- (void) drawTabViewRect: (NSRect)rect
		  inView: (NSView *)view
	       withItems: (NSArray *)items
	    selectedItem: (NSTabViewItem *)selected
{
  NSGraphicsContext *ctxt = GSCurrentContext();
  int howMany = [items count];
  int i;
  int previousState = 0;
  NSRect bounds = [view bounds];
  NSRect aRect = bounds;
  NSColor *lineColour = [NSColor highlightColor];
  NSColor *backgroundColour = [[view window] backgroundColor];
  BOOL truncate = [(NSTabView *)view allowsTruncatedLabels];
  NSTabViewType type = [(NSTabView *)view tabViewType];

  // Make sure some tab is selected
  if (!selected && howMany > 0)
    [(NSTabView *)view selectFirstTabViewItem: nil];

  DPSgsave(ctxt);

  switch (type)
    {
      default:
      case NSTopTabsBezelBorder: 
        aRect.size.height -= 16;
        [self drawButton: aRect withClip: rect];
        break;

      case NSBottomTabsBezelBorder: 
        aRect.size.height -= 16;
        aRect.origin.y += 16;
        [self drawButton: aRect withClip: rect];
        aRect.origin.y -= 16;
        break;

      case NSLeftTabsBezelBorder: 
        aRect.size.width -= 18;
        aRect.origin.x += 18;
        [self drawButton: aRect withClip: rect];
        break;

      case NSRightTabsBezelBorder: 
        aRect.size.width -= 18;
        [self drawButton: aRect withClip: rect];
        break;

      case NSNoTabsBezelBorder: 
        [self drawButton: aRect withClip: rect];
        break;

      case NSNoTabsLineBorder: 
        [[NSColor controlDarkShadowColor] set];
        NSFrameRect(aRect);
        break;

      case NSNoTabsNoBorder: 
        break;
    }

  if (type == NSBottomTabsBezelBorder)
    {
      NSPoint iP;

      iP.x = bounds.origin.x;
      iP.y = bounds.origin.y;
              
      for (i = 0; i < howMany; i++) 
        {
          NSRect r;
          NSRect fRect;
          NSTabViewItem *anItem = [items objectAtIndex: i];
          NSTabState itemState = [anItem tabState];
          NSSize s = [anItem sizeOfLabel: truncate];
          
          [backgroundColour set];

          if (i == 0)
            {
              if (itemState == NSSelectedTab)
                {
                  iP.y += 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabDownSelectedLeft"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y -= 1;
                }
              else if (itemState == NSBackgroundTab)
                {
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabDownUnSelectedLeft"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                }
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }
          else
            {
              if (itemState == NSSelectedTab) 
                {
                  iP.y += 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed:
                    @"common_TabDownUnSelectedToSelectedJunction"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y -= 1;
                }
              else if (itemState == NSBackgroundTab)
                {
                  if (previousState == NSSelectedTab)
                    {
                      iP.y += 1;
                      NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                      [[NSImage imageNamed:
                        @"common_TabDownSelectedToUnSelectedJunction"]
                        compositeToPoint: iP operation: NSCompositeSourceOver];
                      iP.y -= 1;
                    }
                  else
                    {
                      NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                      [[NSImage imageNamed:
                        @"common_TabDownUnSelectedJunction"]
                        compositeToPoint: iP operation: NSCompositeSourceOver];
                    }
                } 
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }  

          r.origin.x = iP.x + 13;
          r.origin.y = iP.y + 2;
          r.size.width = s.width;
          r.size.height = 15;

          fRect = r;
          if (itemState == NSSelectedTab)
            {
              // Undraw the line that separates the tab from its view.
              fRect.origin.y += 1;
              fRect.size.height += 1;
            }
          NSRectFill(fRect);

          // Draw the line at the bottom of the item
          [lineColour set];
          DPSsetlinewidth(ctxt, 1);
          DPSmoveto(ctxt, r.origin.x, r.origin.y - 1);
          DPSrlineto(ctxt, r.size.width, 0);
          DPSstroke(ctxt);
          
          // Label
          [anItem drawLabel: truncate inRect: r];
          
          iP.x += s.width + 13;
          previousState = itemState;

          if (i == howMany - 1)
            {
              [backgroundColour set];

              if ([anItem tabState] == NSSelectedTab)
                {
                  iP.y += 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabDownSelectedRight"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y -= 1;
                }
              else if ([anItem tabState] == NSBackgroundTab)
                {
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabDownUnSelectedRight"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                }
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }
        }
    }
  else if (type == NSTopTabsBezelBorder)
    {
      NSPoint iP;

      iP.x = bounds.origin.x;
      // FIXME: Why not NSMaxY(bounds)?
      iP.y = bounds.size.height - 16;

      for (i = 0; i < howMany; i++) 
        {
          NSRect r;
          NSRect fRect;
          NSTabViewItem *anItem = [items objectAtIndex: i];
          NSTabState itemState = [anItem tabState];
          NSSize s = [anItem sizeOfLabel: truncate];

          [backgroundColour set];
          
          if (i == 0)
            {
              if (itemState == NSSelectedTab)
                {
                  iP.y -= 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabSelectedLeft"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y += 1;
                }
              else if (itemState == NSBackgroundTab)
                {
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabUnSelectedLeft"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                }
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }
          else
            {
              if (itemState == NSSelectedTab)
                {
                  iP.y -= 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed:
                    @"common_TabUnSelectToSelectedJunction"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y += 1;
                }
              else if (itemState == NSBackgroundTab)
                {
                  if (previousState == NSSelectedTab)
                    {
                      iP.y -= 1;
                      NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                      [[NSImage imageNamed:
                        @"common_TabSelectedToUnSelectedJunction"]
                        compositeToPoint: iP operation: NSCompositeSourceOver];
                      iP.y += 1;
                    }
                  else
                    {
                      NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                      [[NSImage imageNamed:
                        @"common_TabUnSelectedJunction"]
                        compositeToPoint: iP operation: NSCompositeSourceOver];
                    }
                } 
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }  

          r.origin.x = iP.x + 13;
          r.origin.y = iP.y;
          r.size.width = s.width;
          r.size.height = 15;
          
          fRect = r;
          if (itemState == NSSelectedTab)
            {
              // Undraw the line that separates the tab from its view.
              fRect.origin.y -= 1;
              fRect.size.height += 1;
            }
          NSRectFill(fRect);

          // Draw the line at the top of the item
          [lineColour set];
          DPSsetlinewidth(ctxt, 1);
          DPSmoveto(ctxt, r.origin.x, r.origin.y + 16);
          DPSrlineto(ctxt, r.size.width, 0);
          DPSstroke(ctxt);
          
          // Label
          [anItem drawLabel: truncate inRect: r];
          
          iP.x += s.width + 13;
          previousState = itemState;

          if (i == howMany - 1)
            {
              [backgroundColour set];
            
              if ([anItem tabState] == NSSelectedTab)
                {              
                  iP.y -= 1;
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabSelectedRight"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                  iP.y += 1;
                }  
              else if ([anItem tabState] == NSBackgroundTab)
                {
                  NSRectFill(NSMakeRect(iP.x, iP.y, 14, 17));
                  [[NSImage imageNamed: @"common_TabUnSelectedRight"]
                    compositeToPoint: iP operation: NSCompositeSourceOver];
                }
              else
                NSLog(@"Not finished yet. Luff ya.\n");
            }
        }
    }
  // FIXME: Missing drawing code for other cases

  DPSgrestore(ctxt);
}

- (void) drawScrollerRect: (NSRect)rect
		   inView: (NSView *)view
		  hitPart: (NSScrollerPart)hitPart
	     isHorizontal: (BOOL)isHorizontal
{
  NSRect rectForPartIncrementLine;
  NSRect rectForPartDecrementLine;
  NSRect rectForPartKnobSlot;
  NSScroller *scroller = (NSScroller *)view;

  rectForPartIncrementLine = [scroller rectForPart: NSScrollerIncrementLine];
  rectForPartDecrementLine = [scroller rectForPart: NSScrollerDecrementLine];
  rectForPartKnobSlot = [scroller rectForPart: NSScrollerKnobSlot];

  [[[view window] backgroundColor] set];
  NSRectFill (rect);

  if (NSIntersectsRect (rect, rectForPartKnobSlot) == YES)
    {
      [scroller drawKnobSlot];
      [scroller drawKnob];
    }

  if (NSIntersectsRect (rect, rectForPartDecrementLine) == YES)
    {
      [scroller drawArrow: NSScrollerDecrementArrow 
		highlight: hitPart == NSScrollerDecrementLine];
    }
  if (NSIntersectsRect (rect, rectForPartIncrementLine) == YES)
    {
      [scroller drawArrow: NSScrollerIncrementArrow 
		highlight: hitPart == NSScrollerIncrementLine];
    }
}

- (void) drawBrowserRect: (NSRect)rect
		  inView: (NSView *)view
	withScrollerRect: (NSRect)scrollerRect
	      columnSize: (NSSize)columnSize
{
  NSBrowser *browser = (NSBrowser *)view;
  NSRect bounds = [view bounds];

  // Load the first column if not already done
  if (![browser isLoaded])
    {
      [browser loadColumnZero];
    }

  // Draws titles
  if ([browser isTitled])
    {
      int i;

      for (i = [browser firstVisibleColumn]; 
	   i <= [browser lastVisibleColumn]; 
	   ++i)
        {
          NSRect titleRect = [browser titleFrameOfColumn: i];
          if (NSIntersectsRect (titleRect, rect) == YES)
            {
              [browser drawTitleOfColumn: i
                    inRect: titleRect];
            }
        }
    }

  // Draws scroller border
  if ([browser hasHorizontalScroller] && 
      [browser separatesColumns])
    {
      NSRect scrollerBorderRect = scrollerRect;
      NSSize bs = [self sizeForBorderType: NSBezelBorder];

      scrollerBorderRect.origin.x = 0;
      scrollerBorderRect.origin.y = 0;
      scrollerBorderRect.size.width += 2 * bs.width;
      scrollerBorderRect.size.height += (2 * bs.height) - 1;

      if ((NSIntersectsRect (scrollerBorderRect, rect) == YES) && [view window])
        {
          [self drawGrayBezel: scrollerBorderRect withClip: rect];
        }
    }

  if (![browser separatesColumns])
    {
      NSPoint p1,p2;
      int     i, visibleColumns;
      float   hScrollerWidth = [browser hasHorizontalScroller] ? 
	[NSScroller scrollerWidth] : 0;
      
      // Columns borders
      [self drawGrayBezel: bounds withClip: rect];
      
      [[NSColor blackColor] set];
      visibleColumns = [browser numberOfVisibleColumns]; 
      for (i = 1; i < visibleColumns; i++)
        {
          p1 = NSMakePoint((columnSize.width * i) + 2 + (i-1), 
                           columnSize.height + hScrollerWidth + 2);
          p2 = NSMakePoint((columnSize.width * i) + 2 + (i-1),
                           hScrollerWidth + 2);
          [NSBezierPath strokeLineFromPoint: p1 toPoint: p2];
        }

      // Horizontal scroller border
      if ([browser hasHorizontalScroller])
        {
          p1 = NSMakePoint(2, hScrollerWidth + 2);
          p2 = NSMakePoint(rect.size.width - 2, hScrollerWidth + 2);
          [NSBezierPath strokeLineFromPoint: p1 toPoint: p2];
        }
    }
}

- (void) drawMenuRect: (NSRect)rect
	       inView: (NSView *)view
	 isHorizontal: (BOOL)horizontal
	    itemCells: (NSArray *)itemCells
{
  int         i = 0;
  int         howMany = [itemCells count];
  NSMenuView *menuView = (NSMenuView *)view;
  NSRect      bounds = [view bounds];

  [self drawBackgroundForMenuView: menuView
	withFrame: bounds
	dirtyRect: rect
	horizontal: horizontal];
  
  // Draw the menu cells.
  for (i = 0; i < howMany; i++)
    {
      NSRect aRect;
      NSMenuItemCell *aCell;
      
      aRect = [menuView rectOfItemAtIndex: i];
      if (NSIntersectsRect(rect, aRect) == YES)
        {
          aCell = [menuView menuItemCellForItemAtIndex: i];
          [aCell drawWithFrame: aRect inView: menuView];
        }
    }
}

- (void) drawScrollViewRect: (NSRect)rect
		     inView: (NSView *)view 
{
  NSScrollView  *scrollView = (NSScrollView *)view;
  NSGraphicsContext *ctxt = GSCurrentContext();
  GSTheme	*theme = [GSTheme theme];
  NSColor	*color;
  NSString	*name;
  NSBorderType   borderType = [scrollView borderType];
  NSRect         bounds = [view bounds];
  BOOL hasInnerBorder = ![[NSUserDefaults standardUserDefaults]
			   boolForKey: @"GSScrollViewNoInnerBorder"];

  name = [theme nameForElement: self];
  if (name == nil)
    {
      name = @"NSScrollView";
    }
  color = [theme colorNamed: name state: GSThemeNormalState];
  if (color == nil)
    {
      color = [NSColor controlDarkShadowColor];
    }
  
  switch (borderType)
    {
      case NSNoBorder:
        break;

      case NSLineBorder:
        [color set];
        NSFrameRect(bounds);
        break;

      case NSBezelBorder:
        [theme drawGrayBezel: bounds withClip: rect];
        break;

      case NSGrooveBorder:
        [theme drawGroove: bounds withClip: rect];
        break;
    }

  if (hasInnerBorder)
    {
      NSScroller *vertScroller = [scrollView verticalScroller];
      NSScroller *horizScroller = [scrollView horizontalScroller];
      CGFloat scrollerWidth = [NSScroller scrollerWidth];

      [color set];
      DPSsetlinewidth(ctxt, 1);

      if ([scrollView hasVerticalScroller])
	{
	  NSInterfaceStyle style;

	  style = NSInterfaceStyleForKey(@"NSScrollViewInterfaceStyle", nil);
	  if (style == NSMacintoshInterfaceStyle
	      || style == NSWindows95InterfaceStyle)
	    {
	      DPSmoveto(ctxt, [vertScroller frame].origin.x - 1, 
			[vertScroller frame].origin.y - 1);
	    }
	  else
	    {
	      DPSmoveto(ctxt, [vertScroller frame].origin.x + scrollerWidth, 
			[vertScroller frame].origin.y - 1);
	    }
	  DPSrlineto(ctxt, 0, [vertScroller frame].size.height + 1);
	  DPSstroke(ctxt);
	}

      if ([scrollView hasHorizontalScroller])
	{
	  float ypos;
	  float scrollerY = [horizScroller frame].origin.y;

	  if ([scrollView isFlipped])
	    {
	      ypos = scrollerY - 1;
	    }
	  else
	    {
	      ypos = scrollerY + scrollerWidth + 1;
	    }

	  DPSmoveto(ctxt, [horizScroller frame].origin.x - 1, ypos);
	  DPSrlineto(ctxt, [horizScroller frame].size.width + 1, 0);
	  DPSstroke(ctxt);
	}
    }
}

- (void) drawBarInside: (NSRect)rect
		inCell: (NSCell *)cell
	       flipped: (BOOL)flipped
{
  NSSliderType type = [(NSSliderCell *)cell sliderType];
  if (type == NSLinearSlider)
    {
      [[NSColor scrollBarColor] set];
      NSRectFill(rect);
    }
}

- (void) drawKnobInCell: (NSCell *)cell
{
  NSView *controlView = [cell controlView];
  NSSliderCell *sliderCell = (NSSliderCell *)cell;

  [sliderCell drawKnob: 
		[sliderCell knobRectFlipped: 
			      [controlView isFlipped]]];  
}

- (NSRect) tableHeaderCellDrawingRectForBounds: (NSRect)theRect
{
  NSSize borderSize;

  // This adjustment must match the drawn border
  borderSize = NSMakeSize(1, 1);

  return NSInsetRect(theRect, borderSize.width, borderSize.height);
}

- (void)drawTableHeaderRect: (NSRect)aRect
		     inView: (NSView *)view
{
  NSTableHeaderView *tableHeaderView = (NSTableHeaderView *)view;
  NSTableView *tableView = [tableHeaderView tableView];
  NSArray *columns;
  int firstColumnToDraw;
  int lastColumnToDraw;
  NSRect drawingRect;
  NSTableColumn *column;
  NSTableColumn *highlightedTableColumn;
  float width;
  int i;
  NSCell *cell;

  if (tableView == nil)
    return;

  firstColumnToDraw = [tableHeaderView columnAtPoint: NSMakePoint (aRect.origin.x,
                                                        aRect.origin.y)];
  if (firstColumnToDraw == -1)
    firstColumnToDraw = 0;

  lastColumnToDraw = [tableHeaderView columnAtPoint: NSMakePoint (NSMaxX (aRect),
                                                       aRect.origin.y)];
  if (lastColumnToDraw == -1)
    lastColumnToDraw = [tableView numberOfColumns] - 1;

  drawingRect = [tableHeaderView headerRectOfColumn: firstColumnToDraw];
  
  columns = [tableView tableColumns];
  highlightedTableColumn = [tableView highlightedTableColumn];
  
  for (i = firstColumnToDraw; i < lastColumnToDraw; i++)
    {
      column = [columns objectAtIndex: i];
      width = [column width];
      drawingRect.size.width = width;
      cell = [column headerCell];
      if ((column == highlightedTableColumn)
          || [tableView isColumnSelected: i])
        {
          [cell setHighlighted: YES];
        }
      else
        {
          [cell setHighlighted: NO];
        }
      [cell drawWithFrame: drawingRect
                           inView: tableHeaderView];
      drawingRect.origin.x += width;
    }
  if (lastColumnToDraw == [tableView numberOfColumns] - 1)
    {
      column = [columns objectAtIndex: lastColumnToDraw];
      width = [column width] - 1;
      drawingRect.size.width = width;
      cell = [column headerCell];
      if ((column == highlightedTableColumn)
          || [tableView isColumnSelected: lastColumnToDraw])
        {
          [cell setHighlighted: YES];
        }
      else
        {
          [cell setHighlighted: NO];
        }
      [cell drawWithFrame: drawingRect
                           inView: tableHeaderView];
      drawingRect.origin.x += width;
    }
  else
    {
      column = [columns objectAtIndex: lastColumnToDraw];
      width = [column width];
      drawingRect.size.width = width;
      cell = [column headerCell];
      if ((column == highlightedTableColumn)
          || [tableView isColumnSelected: lastColumnToDraw])
        {
          [cell setHighlighted: YES];
        }
      else
        {
          [cell setHighlighted: NO];
        }
      [cell drawWithFrame: drawingRect
                           inView: tableHeaderView];
      drawingRect.origin.x += width;
    }
}

- (void) drawPopUpButtonCellInteriorWithFrame: (NSRect)cellFrame
				     withCell: (NSCell *)cell
				       inView: (NSView *)controlView
{
  // Default implementation of this method does nothing.
}

- (void) drawTableViewBackgroundInClipRect: (NSRect)clipRect
				    inView: (NSView *)view
		       withBackgroundColor: (NSColor *)backgroundColor
{

  [backgroundColor set];
  NSRectFill (clipRect);
}

- (void) drawTableViewGridInClipRect: (NSRect)aRect
			      inView: (NSView *)view
{
  NSTableView *tableView = (NSTableView *)view;
  NSRect bounds = [view bounds];
  float minX = NSMinX (aRect);
  float maxX = NSMaxX (aRect);
  float minY = NSMinY (aRect);
  float maxY = NSMaxY (aRect);
  int i;
  float x_pos;
  int startingColumn; 
  int endingColumn;
  int numberOfColumns = [tableView numberOfColumns];
  NSArray *tableColumns = [tableView tableColumns];
  NSGraphicsContext *ctxt = GSCurrentContext ();
  float position = 0.0;
  float *columnOrigins = [tableView _columnOrigins];
  int startingRow    = [tableView rowAtPoint: 
			       NSMakePoint (bounds.origin.x, minY)];
  int endingRow      = [tableView rowAtPoint: 
			       NSMakePoint (bounds.origin.x, maxY)];
  NSColor *gridColor = [tableView gridColor];
  int rowHeight = [tableView rowHeight];
  int numberOfRows = [tableView numberOfRows];

  /* Using columnAtPoint:, rowAtPoint: here calls them only twice 
     per drawn rect */
  x_pos = minX;
  i = 0;
  while ((i < numberOfColumns) && (x_pos > columnOrigins[i]))
    {
      i++;
    }
  startingColumn = (i - 1);

  x_pos = maxX;
  // Nota Bene: we do *not* reset i
  while ((i < numberOfColumns) && (x_pos > columnOrigins[i]))
    {
      i++;
    }
  endingColumn = (i - 1);

  if (endingColumn == -1)
    endingColumn = numberOfColumns - 1;
  /*
  int startingColumn = [tableView columnAtPoint: 
			       NSMakePoint (minX, bounds.origin.y)];
  int endingColumn   = [tableView columnAtPoint: 
			       NSMakePoint (maxX, bounds.origin.y)];
  */

  DPSgsave (ctxt);
  DPSsetlinewidth (ctxt, 1);
  [gridColor set];

  if (numberOfRows > 0)
    {
      /* Draw horizontal lines */
      if (startingRow == -1)
	startingRow = 0;
      if (endingRow == -1)
	endingRow = numberOfRows - 1;
      
      position = bounds.origin.y;
      position += startingRow * rowHeight;
      for (i = startingRow; i <= endingRow + 1; i++)
	{
	  DPSmoveto (ctxt, minX, position);
	  DPSlineto (ctxt, maxX, position);
	  DPSstroke (ctxt);
	  position += rowHeight;
	}
    }
  
  if (numberOfColumns > 0)
    {
      int lastRowPosition = position - rowHeight;
      /* Draw vertical lines */
      if (startingColumn == -1)
	startingColumn = 0;
      if (endingColumn == -1)
	endingColumn = numberOfColumns - 1;

      for (i = startingColumn; i <= endingColumn; i++)
	{
	  DPSmoveto (ctxt, columnOrigins[i], minY);
	  DPSlineto (ctxt, columnOrigins[i], lastRowPosition);
	  DPSstroke (ctxt);
	}
      position =  columnOrigins[endingColumn];
      position += [[tableColumns objectAtIndex: endingColumn] width];  
      /* Last vertical line must moved a pixel to the left */
      if (endingColumn == (numberOfColumns - 1))
	position -= 1;
      DPSmoveto (ctxt, position, minY);
      DPSlineto (ctxt, position, lastRowPosition);
      DPSstroke (ctxt);
    }

  DPSgrestore (ctxt);
}

- (void) drawTableViewRect: (NSRect)aRect
		    inView: (NSView *)view
{
  int startingRow;
  int endingRow;
  int i;
  NSTableView *tableView = (NSTableView *)view;
  int numberOfRows = [tableView numberOfRows];
  int numberOfColumns = [tableView numberOfColumns];
  BOOL drawsGrid = [tableView drawsGrid];

  /* Draw background */
  [tableView drawBackgroundInClipRect: aRect];

  if ((numberOfRows == 0) || (numberOfColumns == 0))
    {
      return;
    }

  /* Draw selection */
  [tableView highlightSelectionInClipRect: aRect];

  /* Draw grid */
  if (drawsGrid)
    {
      [tableView drawGridInClipRect: aRect];
    }
  
  /* Draw visible cells */
  /* Using rowAtPoint: here calls them only twice per drawn rect */
  startingRow = [tableView rowAtPoint: NSMakePoint (0, NSMinY (aRect))];
  endingRow   = [tableView rowAtPoint: NSMakePoint (0, NSMaxY (aRect))];

  if (startingRow == -1)
    {
      startingRow = 0;
    }
  if (endingRow == -1)
    {
      endingRow = numberOfRows - 1;
    }
  //  NSLog(@"drawRect : %d-%d", startingRow, endingRow);
  {
    SEL sel = @selector(drawRow:clipRect:);
    IMP imp = [tableView methodForSelector: sel];
    
    for (i = startingRow; i <= endingRow; i++)
      {
        (*imp)(tableView, sel, i, aRect);
      }
  }
}

- (void) highlightTableViewSelectionInClipRect: (NSRect)clipRect
					inView: (NSView *)view
			      selectingColumns: (BOOL)selectingColumns
{
  NSTableView *tableView = (NSTableView *)view;
  int numberOfRows = [tableView numberOfRows];
  int numberOfColumns = [tableView numberOfColumns];
  NSIndexSet *selectedRows = [tableView selectedRowIndexes];
  NSIndexSet *selectedColumns = [tableView selectedColumnIndexes];
  NSColor *backgroundColor = [tableView backgroundColor];

  if (selectingColumns == NO)
    {
      NSInteger selectedRowsCount;
      NSUInteger row;
      NSInteger startingRow, endingRow;

      selectedRowsCount = [selectedRows count];      
      if (selectedRowsCount == 0)
	return;
      
      /* highlight selected rows */
      startingRow = [tableView rowAtPoint: NSMakePoint(0, NSMinY(clipRect))];
      endingRow   = [tableView rowAtPoint: NSMakePoint(0, NSMaxY(clipRect))];
      
      if (startingRow == -1)
	startingRow = 0;
      if (endingRow == -1)
	endingRow = numberOfRows - 1;
      
      row = [selectedRows indexGreaterThanOrEqualToIndex: startingRow];
      while ((row != NSNotFound) && (row <= endingRow))
	{
	  NSColor *selectionColor = nil;
	  
	  // Switch to the alternate color of the backgroundColor is white.
	  if([backgroundColor isEqual: [NSColor whiteColor]])
	    {
	      selectionColor = [NSColor colorWithCalibratedRed: 0.86
					green: 0.92
					blue: 0.99
					alpha: 1.0];
	    }
	  else
	    {
	      selectionColor = [NSColor whiteColor];
	    }

	  //NSHighlightRect(NSIntersectionRect([tableView rectOfRow: row],
	  //						 clipRect));
	  [selectionColor set];
	  NSRectFill(NSIntersectionRect([tableView rectOfRow: row], clipRect));
	  row = [selectedRows indexGreaterThanIndex: row];
	}	  
    }
  else // Selecting columns
    {
      NSUInteger selectedColumnsCount;
      NSUInteger column;
      NSInteger startingColumn, endingColumn;
      
      selectedColumnsCount = [selectedColumns count];
      
      if (selectedColumnsCount == 0)
	return;
      
      /* highlight selected columns */
      startingColumn = [tableView columnAtPoint: NSMakePoint(NSMinX(clipRect), 0)];
      endingColumn = [tableView columnAtPoint: NSMakePoint(NSMaxX(clipRect), 0)];

      if (startingColumn == -1)
	startingColumn = 0;
      if (endingColumn == -1)
	endingColumn = numberOfColumns - 1;

      column = [selectedColumns indexGreaterThanOrEqualToIndex: startingColumn];
      while ((column != NSNotFound) && (column <= endingColumn))
	{
	  NSHighlightRect(NSIntersectionRect([tableView rectOfColumn: column],
					     clipRect));
	  column = [selectedColumns indexGreaterThanIndex: column];
	}	  
    }
}

- (void) drawTableViewRow: (int)rowIndex 
		 clipRect: (NSRect)clipRect
		   inView: (NSView *)view
{
  NSTableView *tableView = (NSTableView *)view;
  // int numberOfRows = [tableView numberOfRows];
  int numberOfColumns = [tableView numberOfColumns];
  // NSIndexSet *selectedRows = [tableView selectedRowIndexes];
  // NSColor *backgroundColor = [tableView backgroundColor];
  id dataSource = [tableView dataSource];
  float *columnOrigins = [tableView _columnOrigins];
  int editedRow = [tableView editedRow];
  int editedColumn = [tableView editedColumn];
  NSArray *tableColumns = [tableView tableColumns];
  int startingColumn; 
  int endingColumn;
  NSTableColumn *tb;
  NSRect drawingRect;
  NSCell *cell;
  int i;
  float x_pos;

  if (dataSource == nil)
    {
      return;
    }

  /* Using columnAtPoint: here would make it called twice per row per drawn 
     rect - so we avoid it and do it natively */

  /* Determine starting column as fast as possible */
  x_pos = NSMinX (clipRect);
  i = 0;
  while ((i < numberOfColumns) && (x_pos > columnOrigins[i]))
    {
      i++;
    }
  startingColumn = (i - 1);

  if (startingColumn == -1)
    startingColumn = 0;

  /* Determine ending column as fast as possible */
  x_pos = NSMaxX (clipRect);
  // Nota Bene: we do *not* reset i
  while ((i < numberOfColumns) && (x_pos > columnOrigins[i]))
    {
      i++;
    }
  endingColumn = (i - 1);

  if (endingColumn == -1)
    endingColumn = numberOfColumns - 1;

  /* Draw the row between startingColumn and endingColumn */
  for (i = startingColumn; i <= endingColumn; i++)
    {
      tb = [tableColumns objectAtIndex: i];
      cell = [tb dataCellForRow: rowIndex];
      if (i == editedColumn && rowIndex == editedRow)
	[cell _setInEditing: YES];
      [tableView _willDisplayCell: cell
		 forTableColumn: tb
		 row: rowIndex];
      [cell setObjectValue: [dataSource tableView: tableView
					objectValueForTableColumn: tb
					row: rowIndex]]; 
      drawingRect = [tableView frameOfCellAtColumn: i
			       row: rowIndex];
      [cell drawWithFrame: drawingRect inView: tableView];
      if (i == editedColumn && rowIndex == editedRow)
	[cell _setInEditing: NO];
    }
}
@end
