/** <title>NSTabView</title>

   <abstract>The tabular view class</abstract>

   Copyright (C) 1999,2000 Free Software Foundation, Inc.

   Author: Michael Hanni <mhanni@sprintmail.com>
   Date: 1999

   This file is part of the GNUstep GUI Library.

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

#import <Foundation/NSKeyValueCoding.h>
#import <Foundation/NSValue.h>
#import "AppKit/NSColor.h"
#import "AppKit/NSEvent.h"
#import "AppKit/NSFont.h"
#import "AppKit/NSFontManager.h"
#import "AppKit/NSForm.h"
#import "AppKit/NSGraphics.h"
#import "AppKit/NSImage.h"
#import "AppKit/NSKeyValueBinding.h"
#import "AppKit/NSMatrix.h"
#import "AppKit/NSTabView.h"
#import "AppKit/NSTabViewItem.h"
#import "AppKit/NSWindow.h"
#import "GNUstepGUI/GSTheme.h"
#import "GSBindingHelpers.h"
#import "NSViewPrivate.h"

@interface NSTabViewItem (KeyViewLoop)
- (void) _setUpKeyViewLoopWithNextKeyView: (NSView *)nextKeyView;
- (NSView *) _lastKeyView;
@end

@implementation NSTabView

/*
 * Class methods
 */
+ (void) initialize
{
  if (self == [NSTabView class])
    {
      [self setVersion: 3];

      [self exposeBinding: NSSelectedIndexBinding];
      [self exposeBinding: NSFontBinding];
      [self exposeBinding: NSFontNameBinding];
      [self exposeBinding: NSFontSizeBinding];
    }
}

- (id) initWithFrame: (NSRect)rect
{
  self = [super initWithFrame: rect];

  if (self)
    {
      // setup variables  
      ASSIGN(_items, [NSMutableArray array]);
      ASSIGN(_font, [NSFont systemFontOfSize: 0]);
      _selected_item = NSNotFound;
      //_selected = nil;
      //_truncated_label = NO;
    }

  return self;
}

- (void) dealloc
{
  RELEASE(_items);
  RELEASE(_font);
  // Reset the _selected attribute to prevent crash when -dealloc calls
  // -setNextKeyView:
  _selected = nil;
  [super dealloc];
}

/*
// FIXME: This should be defined
- (BOOL) isFlipped
{
  return YES;
}
*/

// tab management.

- (void) addTabViewItem: (NSTabViewItem*)tabViewItem
{
  [self insertTabViewItem: tabViewItem atIndex: [_items count]];
}

- (void) insertTabViewItem: (NSTabViewItem*)tabViewItem
                   atIndex: (NSInteger)index
{
  [tabViewItem _setTabView: self];
  [_items insertObject: tabViewItem atIndex: index];

  if ((_selected_item != NSNotFound) && (index <= _selected_item))
    {
      _selected_item++;
    }

  if ([_delegate respondsToSelector: 
    @selector(tabViewDidChangeNumberOfTabViewItems:)])
    {
      [_delegate tabViewDidChangeNumberOfTabViewItems: self];
    }

  /* TODO (Optimize) - just mark the tabs rect as needing redisplay */
  [self setNeedsDisplay: YES];
}

- (void) removeTabViewItem: (NSTabViewItem*)tabViewItem
{
  NSUInteger i = [_items indexOfObject: tabViewItem];
  
  if (i == NSNotFound)
    return;

  if ([tabViewItem isEqual: _selected])
    {
      // We cannot call [self selectTabViewItem: nil] here as the delegate might refuse this
      [[_selected view] removeFromSuperview];
      _selected = nil;
      _selected_item = NSNotFound;
    }

  [_items removeObjectAtIndex: i];

  if ((_selected_item != NSNotFound) && (i <= _selected_item))
    {
      _selected_item--;
    }

  if ([_delegate respondsToSelector: 
    @selector(tabViewDidChangeNumberOfTabViewItems:)])
    {
      [_delegate tabViewDidChangeNumberOfTabViewItems: self];
    }

  /* TODO (Optimize) - just mark the tabs rect as needing redisplay unless
                       removed tab was selected */
  [self setNeedsDisplay: YES];
}

- (NSInteger) indexOfTabViewItem: (NSTabViewItem*)tabViewItem
{
  return [_items indexOfObject: tabViewItem];
}

- (NSInteger) indexOfTabViewItemWithIdentifier: (id)identifier
{
  NSUInteger howMany = [_items count];
  NSUInteger i;

  for (i = 0; i < howMany; i++)
    {
      id anItem = [_items objectAtIndex: i];

      if ([[anItem identifier] isEqual: identifier])
        return i;
    }

  return NSNotFound;
}

- (NSInteger) numberOfTabViewItems
{
  return [_items count];
}

- (NSTabViewItem*) tabViewItemAtIndex: (NSInteger)index
{
  return [_items objectAtIndex: index];
}

- (NSArray*) tabViewItems
{
  return (NSArray*)_items;
}

- (void) selectFirstTabViewItem: (id)sender
{
  [self selectTabViewItemAtIndex: 0];
}

- (void) selectLastTabViewItem: (id)sender
{
  [self selectTabViewItem: [_items lastObject]];
}

- (void) selectNextTabViewItem: (id)sender
{
  if ((_selected_item != NSNotFound) && ((_selected_item + 1) < [_items count]))
    {
      [self selectTabViewItemAtIndex: _selected_item + 1];
    }
}

- (void) selectPreviousTabViewItem: (id)sender
{
  if ((_selected_item != NSNotFound) && (_selected_item > 0))
    {
      [self selectTabViewItemAtIndex: _selected_item - 1];
    }
}

- (NSTabViewItem*) selectedTabViewItem
{
  // FIXME: Why not just return _selected?
  if (_selected_item == NSNotFound || [_items count] == 0)
    return nil;
  return [_items objectAtIndex: _selected_item];
}

- (void) selectTabViewItem: (NSTabViewItem*)tabViewItem
{
  BOOL canSelect = YES;

  if ([_delegate respondsToSelector: 
    @selector(tabView: shouldSelectTabViewItem:)])
    {
      canSelect = [_delegate tabView: self
                shouldSelectTabViewItem: tabViewItem];
    }

  if (canSelect)
    {
      NSView *selectedView;

      if (_selected != nil)
        {
          [_selected _setTabState: NSBackgroundTab];

          /* NB: If [_selected view] is nil this does nothing, which
             is fine.  */
          [[_selected view] removeFromSuperview];
        }

      if ([_delegate respondsToSelector: 
        @selector(tabView: willSelectTabViewItem:)])
        {
          [_delegate tabView: self willSelectTabViewItem: tabViewItem];
        }

      _selected = tabViewItem;
      _selected_item = [_items indexOfObject: _selected];
      [_selected _setTabState: NSSelectedTab];

      selectedView = [_selected view];

      if (selectedView != nil)
        {
	  NSView *firstResponder;

          [self addSubview: selectedView];
          // FIXME: We should not change this mask
          [selectedView setAutoresizingMask:
	    NSViewWidthSizable | NSViewHeightSizable];
          [selectedView setFrame: [self contentRect]];
	  firstResponder = [_selected initialFirstResponder];
	  if (firstResponder == nil)
	    {
	      firstResponder = [_selected view];
	      [_selected setInitialFirstResponder: firstResponder];
	      [firstResponder _setUpKeyViewLoopWithNextKeyView:
		_original_nextKeyView];
	    }
	  [self setNextKeyView: firstResponder];
          [_window makeFirstResponder: firstResponder];
        }
      
      /* Will need to redraw tabs and content area. */
      [self setNeedsDisplay: YES];
      
      if ([_delegate respondsToSelector: 
        @selector(tabView: didSelectTabViewItem:)])
        {
          [_delegate tabView: self didSelectTabViewItem: _selected];
        }
    }
}

- (void) selectTabViewItemAtIndex: (NSInteger)index
{
  if (index < 0 || index >= [_items count])
    [self selectTabViewItem: nil];
  else
    [self selectTabViewItem: [_items objectAtIndex: index]];
}

- (void) selectTabViewItemWithIdentifier: (id)identifier 
{
  NSInteger index = [self indexOfTabViewItemWithIdentifier: identifier];

  [self selectTabViewItemAtIndex: index];
}

- (void) takeSelectedTabViewItemFromSender: (id)sender
{
  NSInteger index = -1;

  if ([sender respondsToSelector: @selector(indexOfSelectedItem)] == YES)
    {
      index = [sender indexOfSelectedItem];
    }
  else if ([sender isKindOfClass: [NSMatrix class]] == YES)
    {
      NSInteger cols = [sender numberOfColumns];
      NSInteger row = [sender selectedRow];
      NSInteger col = [sender selectedColumn];

      if (row >= 0 && col >= 0)
        {
          index = row * cols + col;
        }
    }
  [self selectTabViewItemAtIndex: index];
}

- (void) setFont: (NSFont*)font
{
  ASSIGN(_font, font);
}

- (NSFont*) font
{
  return _font;
}

- (void) setTabViewType: (NSTabViewType)tabViewType
{
  _type = tabViewType;
}

- (NSTabViewType) tabViewType
{
  return _type;
}

- (void) setDrawsBackground: (BOOL)flag
{
  _draws_background = flag;
}

- (BOOL) drawsBackground
{
  return _draws_background;
}

- (void) setAllowsTruncatedLabels: (BOOL)allowTruncatedLabels
{
  _truncated_label = allowTruncatedLabels;
}

- (BOOL) allowsTruncatedLabels
{
  return _truncated_label;
}

- (void) setDelegate: (id)anObject
{
  _delegate = anObject;
}

- (id) delegate
{
  return _delegate;
}

// content and size

- (NSSize) minimumSize
{
  // FIXME: This should allow some space for the tabs
  switch (_type)
    {
      case NSTopTabsBezelBorder:
        return NSMakeSize(2, 19.5);
      case NSNoTabsBezelBorder:
        return NSMakeSize(2, 3);
      case NSNoTabsLineBorder:
        return NSMakeSize(2, 3);
      case NSBottomTabsBezelBorder:
        return NSMakeSize(2, 16);
      case NSLeftTabsBezelBorder:
        return NSMakeSize(16, 3);
      case NSRightTabsBezelBorder:
        return NSMakeSize(16, 3);
      case NSNoTabsNoBorder:
      default:
        return NSZeroSize;
    }
}

- (NSRect) contentRect
{
  NSRect cRect = _bounds;

  /* 
     FIXME: All these numbers seem wrong to me.
     For a bezel border we loose 2 pixel on each side, 
     for a line border 1 pixel. On top of that we will 
     need the space for the tab.
  */
  switch (_type)
    {
      case NSTopTabsBezelBorder:
        cRect.origin.y += 1; 
        cRect.origin.x += 0.5; 
        cRect.size.width -= 2;
        cRect.size.height -= 18.5;
        break;
      case NSNoTabsBezelBorder:
        cRect.origin.y += 1; 
        cRect.origin.x += 0.5; 
        cRect.size.width -= 2;
        cRect.size.height -= 2;
        break;
      case NSNoTabsLineBorder:
        cRect.origin.y += 1; 
        cRect.origin.x += 0.5; 
        cRect.size.width -= 2;
        cRect.size.height -= 2;
        break;
    case NSBottomTabsBezelBorder:
        cRect.size.height -= 8;
        cRect.origin.y = 8;
        break;
      case NSLeftTabsBezelBorder:
        cRect.size.width -= 16;
        cRect.origin.x += 16;
        break;
      case NSRightTabsBezelBorder:
        cRect.size.width -= 16;
        break;
      case NSNoTabsNoBorder:
      default:
        break;
    }

  return cRect;
}

// Drawing.

- (void) drawRect: (NSRect)rect
{
  [[GSTheme theme] drawTabViewRect: rect
		   inView: self
		   withItems: _items
		   selectedItem: _selected];
}

- (BOOL) isOpaque
{
  return NO;
}

// Event handling.

/* 
 *  Find the tab view item containing the NSPoint point. This point 
 *  is expected to be alreay in the coordinate system of the tab view.
 */
- (NSTabViewItem*) tabViewItemAtPoint: (NSPoint)point
{
  NSInteger howMany = [_items count];
  NSInteger i;

  for (i = 0; i < howMany; i++)
    {
      NSTabViewItem *anItem = [_items objectAtIndex: i];

      if (NSPointInRect(point, [anItem _tabRect]))
        return anItem;
    }

  return nil;
}

- (void) mouseDown: (NSEvent *)theEvent
{
  NSPoint location = [self convertPoint: [theEvent locationInWindow] 
                           fromView: nil];
  NSTabViewItem *anItem = [self tabViewItemAtPoint: location];
  
  if (anItem != nil  &&  ![anItem isEqual: _selected])
    {
      GSKeyValueBinding *theBinding;

      [self selectTabViewItem: anItem];

      theBinding = [GSKeyValueBinding getBinding: NSSelectedIndexBinding 
                                       forObject: self];
      if (theBinding != nil)
        [theBinding reverseSetValueFor: NSSelectedIndexBinding];
    }
}


- (NSControlSize) controlSize
{
  // FIXME
  return NSRegularControlSize;
}

/**
 * Not implemented.
 */
- (void) setControlSize: (NSControlSize)controlSize
{
  // FIXME 
}

- (NSControlTint) controlTint
{
  // FIXME
  return NSDefaultControlTint;
}

/**
 * Not implemented.
 */
- (void) setControlTint: (NSControlTint)controlTint
{
  // FIXME 
}

// Coding.

- (void) encodeWithCoder: (NSCoder*)aCoder
{ 
  [super encodeWithCoder: aCoder];
  if ([aCoder allowsKeyedCoding])
    {
      unsigned int type = _type; // no flags set...

      [aCoder encodeBool: [self allowsTruncatedLabels] forKey: @"NSAllowTruncatedLabels"];
      [aCoder encodeBool: [self drawsBackground] forKey: @"NSDrawsBackground"];
      [aCoder encodeObject: [self font] forKey: @"NSFont"];
      [aCoder encodeObject: _items forKey: @"NSTabViewItems"];
      [aCoder encodeObject: [self selectedTabViewItem] forKey: @"NSSelectedTabViewItem"];
      [aCoder encodeInt: type forKey: @"NSTvFlags"];
    }
  else
    {
      [aCoder encodeObject: _items];
      [aCoder encodeObject: _font];
      [aCoder encodeValueOfObjCType: @encode(int) at: &_type];
      [aCoder encodeValueOfObjCType: @encode(BOOL) at: &_draws_background];
      [aCoder encodeValueOfObjCType: @encode(BOOL) at: &_truncated_label];
      [aCoder encodeConditionalObject: _delegate];
      [aCoder encodeValueOfObjCType: @encode(NSUInteger) at: &_selected_item];
    }
}

- (id) initWithCoder: (NSCoder*)aDecoder
{
  self = [super initWithCoder: aDecoder];

  _selected_item = NSNotFound;

  if ([aDecoder allowsKeyedCoding])
    {
      if ([aDecoder containsValueForKey: @"NSAllowTruncatedLabels"])
        {
          [self setAllowsTruncatedLabels: [aDecoder decodeBoolForKey: 
                                                        @"NSAllowTruncatedLabels"]];
        }
      if ([aDecoder containsValueForKey: @"NSDrawsBackground"])
        {
          [self setDrawsBackground: [aDecoder decodeBoolForKey: 
                                                  @"NSDrawsBackground"]];
        }
      if ([aDecoder containsValueForKey: @"NSFont"])
        {
          [self setFont: [aDecoder decodeObjectForKey: @"NSFont"]];
        }
      if ([aDecoder containsValueForKey: @"NSTvFlags"])
        {
          int vFlags = [aDecoder decodeIntForKey: @"NSTvFlags"];

          [self setControlTint: ((vFlags & 0x70000000) >> 28)];
          [self setControlSize: ((vFlags & 0x0c000000) >> 26)];
          [self setTabViewType: (vFlags & 0x00000007)];
        }
      if ([aDecoder containsValueForKey: @"NSTabViewItems"])
        {
          ASSIGN(_items, [aDecoder decodeObjectForKey: @"NSTabViewItems"]);
        }
      if ([aDecoder containsValueForKey: @"NSSelectedTabViewItem"])
        {
          [self selectTabViewItem: [aDecoder decodeObjectForKey: 
                                                 @"NSSelectedTabViewItem"]];
        }
    }
  else
    {
      int version = [aDecoder versionForClassName: @"NSTabView"];

      [aDecoder decodeValueOfObjCType: @encode(id) at: &_items];
      [aDecoder decodeValueOfObjCType: @encode(id) at: &_font];
      [aDecoder decodeValueOfObjCType: @encode(int) at: &_type];
      if (version < 2)
        {
          switch(_type)
            {
              case 0:
                _type = NSTopTabsBezelBorder;
                break;
              case 5:
                _type = NSLeftTabsBezelBorder;
                break;
              case 1:
                _type = NSBottomTabsBezelBorder;
                break;
              case 6:
                _type = NSRightTabsBezelBorder;
                break;
              case 2:
                _type = NSNoTabsBezelBorder;
                break;
              case 3:
                _type = NSNoTabsLineBorder;
                break;
              case 4:
                _type = NSNoTabsNoBorder;
                break;
              default:
                break;
            }
        }
      [aDecoder decodeValueOfObjCType: @encode(BOOL) at: &_draws_background];
      [aDecoder decodeValueOfObjCType: @encode(BOOL) at: &_truncated_label];
      _delegate = [aDecoder decodeObject];
      if (version < 3)
        {
	  int tmp;
          [aDecoder decodeValueOfObjCType: @encode(int) at: &tmp];
          _selected_item = tmp;
	}
      else
	{
          [aDecoder decodeValueOfObjCType: @encode(NSUInteger) at: &_selected_item];
	}
      _selected = [_items objectAtIndex: _selected_item];
    }
  return self;
}

- (void) setValue: (id)anObject forKey: (NSString*)aKey
{
  if ([aKey isEqual: NSSelectedIndexBinding])
    {
      [self selectTabViewItemAtIndex: [anObject intValue]];
    }
  else if ([aKey isEqual: NSFontNameBinding])
    {
      [self setFont: [[NSFontManager sharedFontManager] convertFont: [self font] 
                                                             toFace: anObject]];
    }
  else if ([aKey isEqual: NSFontSizeBinding])
    {
      [self setFont: [[NSFontManager sharedFontManager] convertFont: [self font]
                                                             toSize: [anObject doubleValue]]];
    }
  else
    {
      [super setValue: anObject forKey: aKey];
    }
}

- (id) valueForKey: (NSString*)aKey
{
  if ([aKey isEqual: NSSelectedIndexBinding])
    {
      return [NSNumber numberWithInt: [self indexOfTabViewItem: 
                                              [self selectedTabViewItem]]];
    }
  else if ([aKey isEqual: NSFontNameBinding])
    {
      return [[self font] fontName];
    }
  else if ([aKey isEqual: NSFontSizeBinding])
    {
      return [NSNumber numberWithDouble: (double)[[self font] pointSize]];
    }
  else
    {
      return [super valueForKey: aKey];
    }
}
@end

@implementation NSTabViewItem (KeyViewLoop)

- (void) _setUpKeyViewLoopWithNextKeyView: (NSView *)nextKeyView
{
  [self setInitialFirstResponder: [self view]];
  [[self view] _setUpKeyViewLoopWithNextKeyView: nextKeyView];
}

- (NSView *) _lastKeyView
{
  NSView *keyView = [self initialFirstResponder];
  NSView *itemView = [self view];
  NSView *lastKeyView = nil;
  NSMutableArray *views = // cycle protection
    [[NSMutableArray alloc] initWithCapacity: 1 + [[itemView subviews] count]];

  if (keyView == nil && itemView != nil)
    {
      [self _setUpKeyViewLoopWithNextKeyView: itemView];
    }
  while ([keyView isDescendantOf: itemView] && ![views containsObject: keyView])
    {
      [views addObject: keyView];
      lastKeyView = keyView;
      keyView = [keyView nextKeyView];
    }
  [views release];
  return lastKeyView;
}

@end

@implementation NSTabView (KeyViewLoop)

- (void) _setUpKeyViewLoopWithNextKeyView: (NSView *)nextKeyView
{
  [_items makeObjectsPerform: @selector(_setUpKeyViewLoopWithNextKeyView:)
		  withObject: nextKeyView];
  if (_selected)
    {
      [super setNextKeyView: [_selected initialFirstResponder]];
    }
  [self setNextKeyView: nextKeyView];
}

- (void) setNextKeyView: (NSView *)nextKeyView
{
  _original_nextKeyView = nextKeyView;
  if (_selected)
    {
      [[_selected _lastKeyView] setNextKeyView: nextKeyView];
    }
  else
    {
      [super setNextKeyView: nextKeyView];
    }
}

@end
