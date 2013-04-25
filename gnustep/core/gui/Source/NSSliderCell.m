/** <title>NSSliderCell</title>

   Copyright (C) 1996,1999 Free Software Foundation, Inc.

   Author: Ovidiu Predescu <ovidiu@net-community.com>
   Date: September 1997
   Rewrite: Richard Frith-Macdonald <richard@brainstorm.co.uk>
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

#include <math.h>                  // (float)rintf(float x)
#include "config.h"
#import <Foundation/NSString.h>
#import <Foundation/NSException.h>
#import <Foundation/NSValue.h>

#import "AppKit/NSApplication.h"
#import "AppKit/NSBezierPath.h"
#import "AppKit/NSColor.h"
#import "AppKit/NSControl.h"
#import "AppKit/NSEvent.h"
#import "AppKit/NSGraphics.h"
#import "AppKit/NSImage.h"
#import "AppKit/NSSliderCell.h"
#import "AppKit/NSTextFieldCell.h"
#import "AppKit/NSWindow.h"
#import <GNUstepGUI/GSTheme.h>

#import "GSGuiPrivate.h"

#ifndef HAVE_ATAN2F
#define atan2f atan2
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626434
#endif

static inline
float _floatValueForMousePoint (NSPoint point, NSRect knobRect,
				NSRect slotRect, BOOL isVertical, 
				float minValue, float maxValue, 
				NSSliderCell *theCell, BOOL flipped, 
				BOOL isCircular)
{
  float floatValue = 0;
  float position;

  if (isCircular)
    {
      NSPoint slotCenter = NSMakePoint(NSMidX(slotRect), NSMidY(slotRect));
      NSPoint pointRelativeToKnobCenter = NSMakePoint(point.x - slotCenter.x,
						      point.y - slotCenter.y);
      if (flipped)
	{
	  pointRelativeToKnobCenter.y *= -1.0;
	}
      floatValue = atan2f(pointRelativeToKnobCenter.x,
			  pointRelativeToKnobCenter.y) / (2.0 * M_PI);
      if (floatValue < 0)
	{
	  floatValue += 1.0;
	}
      // floatValue is 0 for up, 0.25 for right, 0.5 for down, 0.75 for left, etc.
    }
  else
    {
      // Adjust the point to lie inside the knob slot. We don't
      // have to worry whether the view is flipped or not.
      if (isVertical)
	{
	  if (point.y < slotRect.origin.y + knobRect.size.height / 2)
	    {
	      position = slotRect.origin.y + knobRect.size.height / 2;
	    }
	  else if (point.y > slotRect.origin.y + slotRect.size.height
		   - knobRect.size.height / 2)
	    {
	      position = slotRect.origin.y + slotRect.size.height
		- knobRect.size.height / 2;
	    }
	  else
	    position = point.y;
	  // Compute the float value
	  floatValue = (position - (slotRect.origin.y + knobRect.size.height/2))
	    / (slotRect.size.height - knobRect.size.height);
	  if (flipped)
	    floatValue = 1 - floatValue;
	}
      else
	{
	  if (point.x < slotRect.origin.x + knobRect.size.width / 2)
	    {
	      position = slotRect.origin.x + knobRect.size.width / 2;
	    }
	  else if (point.x > slotRect.origin.x + slotRect.size.width
		   - knobRect.size.width / 2)
	    {
	      position = slotRect.origin.x + slotRect.size.width
		- knobRect.size.width / 2;
	    }
	  else
	    position = point.x;

	  // Compute the float value given the knob size
	  floatValue = (position - (slotRect.origin.x + knobRect.size.width / 2))
	    / (slotRect.size.width - knobRect.size.width);
	}
    }

  return floatValue * (maxValue - minValue) + minValue;
}

/**
  <unit>
  <heading>Class Description</heading>

  <p>
  An NSSliderCell controls the behaviour and appearance of an
  associated NSSlider, or a single slider in an NSMatrix.  Tick marks
  are defined in the official standard, but are not implemented in
  GNUstep.
  </p>
  <p> 
  An NSSliderCell can be customized through its
  <code>set...</code> methods.  If these do not provide enough
  customization, a subclass can be created, which overrides any of the
  follwing methods: <code>knobRectFlipped:</code>,
  <code>drawBarInside:flipped:</code>, <code>drawKnob:</code>, or
  <code>prefersTrackingUntilMouseUp</code>.
  </p>
  </unit> 
*/
@implementation NSSliderCell

+ (void) initialize
{
  if (self == [NSSliderCell class])
    {
      /* Set the class version to 2, as tick information is now 
	 stored in the encoding */
      [self setVersion: 2];
    }
}

- (id) init
{
  self = [self initImageCell: nil];
  if (self == nil)
    return nil;

  _altIncrementValue = -1;
  _isVertical = -1;
  [self setMinValue: 0];
  [self setMaxValue: 1];
  [self setDoubleValue: 0];
  _cell.is_bordered = YES;
  _cell.is_bezeled = NO;
  [self setContinuous: YES];
  [self setSliderType: NSLinearSlider];
      
  _knobCell = [NSCell new];
  _titleCell = [NSTextFieldCell new];
  [_titleCell setTextColor: [NSColor controlTextColor]];
  [_titleCell setStringValue: @""];
  [_titleCell setAlignment: NSCenterTextAlignment];

  return self;
}

- (void) dealloc
{
  RELEASE(_titleCell);
  RELEASE(_knobCell);
  [super dealloc];
}

- (id) copyWithZone:(NSZone *)zone
{
  NSSliderCell *cpy = [super copyWithZone: zone];

  if (cpy != nil)
    {
      /* since NSCells call to NSCopyObject only copies object addresses */
      cpy->_titleCell = [_titleCell copyWithZone: zone];
      cpy->_knobCell = [_knobCell copyWithZone: zone];
    }

  return cpy;
}

- (BOOL) isContinuous
{
  return (_action_mask & NSLeftMouseDraggedMask) != 0;
}

- (void) setContinuous: (BOOL)flag
{
  if (flag)
    {
      _action_mask |= NSLeftMouseDraggedMask;
    }
  else
    {
      _action_mask &= ~NSLeftMouseDraggedMask;
    }
}

/** <p>Draws the slider's track, not including the bezel, in <var>aRect</var>
  <var>flipped</var> indicates whether the control view has a flipped 
   coordinate system.</p>

  <p>Do not call this method directly, it is provided for subclassing
  only.</p> */
- (void) drawBarInside: (NSRect)rect flipped: (BOOL)flipped
{
  [[GSTheme theme] drawBarInside: rect
		   inCell: self
		   flipped: flipped];
}

/**<p>Returns the rect in which to draw the knob, based on the
  coordinate system of the NSSlider or NSMatrix this NSSliderCell is
  associated with.  <var>flipped</var> indicates whether or not that
  coordinate system is flipped, which can be determined by sending the
  <code>isFlipped</code> message to the associated NSSlider or
  NSMatrix.</p>

  <p>Do not call this method directly.  It is included for subclassing
  only.</p> */
- (NSRect) knobRectFlipped: (BOOL)flipped
{
  NSImage *image = [_knobCell image];
  NSSize size;
  NSPoint origin;
  float floatValue = [self floatValue];

  // FIXME: this method needs to be refactored out to GSTheme

  if (_isVertical && flipped)
    {
      floatValue = _maxValue + _minValue - floatValue;
    }

  floatValue = (floatValue - _minValue) / (_maxValue - _minValue);

  if (image != nil)
    {
      size = [image size];
    }
  else
    {
      size = NSZeroSize;
    }

  if (_isVertical == YES)
    {
      origin = _trackRect.origin;
      origin.y += (_trackRect.size.height - size.height) * floatValue;
    }
  else
    {
      origin = _trackRect.origin;
      origin.x += (_trackRect.size.width - size.width) * floatValue;
    }

  return NSMakeRect (origin.x, origin.y, size.width, size.height); 
}

/** <p>Calculates the rect in which to draw the knob, then calls
  <code>drawKnob:</code> Before calling this method, a
  <code>lockFocus</code> message must be sent to the cell's control
  view.</p>

  <p>When subclassing NSSliderCell, do not override this method.
  Override <code>drawKnob:</code> instead.</p> <p>See Also: -drawKnob:</p>
*/
- (void) drawKnob
{
  [[GSTheme theme] drawKnobInCell: self];
}

/**<p>Draws the knob in <var>knobRect</var>.  Before calling this
  method, a <code>lockFocus</code> message must be sent to the cell's
  control view.</p>

  <p>Do not call this method directly.  It is included for subclassing
  only.</p> <p>See Also: -drawKnob</p>
*/
- (void) drawKnob: (NSRect)knobRect
{
  NSColor* knobBackgroundColor = [NSColor controlBackgroundColor];
  [knobBackgroundColor set];
  NSRectFill (knobRect);
  
  [_knobCell drawInteriorWithFrame: knobRect inView: _control_view];
}

- (void) drawInteriorWithFrame: (NSRect)cellFrame inView: (NSView*)controlView
{
  cellFrame = [self drawingRectForBounds: cellFrame];
  _trackRect = cellFrame;

  if (_type == NSCircularSlider)
    {
      NSBezierPath *circle;
      NSPoint knobCenter;
      NSPoint point;
      NSRect knobRect;
      float fraction, angle, radius;

      if (cellFrame.size.width > cellFrame.size.height)
	{
	  knobRect = NSMakeRect(cellFrame.origin.x + ((cellFrame.size.width -
						       cellFrame.size.height) / 2.0),
				cellFrame.origin.y,
				cellFrame.size.height,
				cellFrame.size.height);
	}
      else
	{
	  knobRect = NSMakeRect(cellFrame.origin.x,
				cellFrame.origin.y + ((cellFrame.size.height -
						       cellFrame.size.width) / 2.0),
				cellFrame.size.width,
				cellFrame.size.width);
	}
      knobCenter = NSMakePoint(NSMidX(knobRect), NSMidY(knobRect));

      circle = [NSBezierPath bezierPathWithOvalInRect: knobRect];
      [[NSColor controlBackgroundColor] set];    
      [circle fill];
      [[NSColor blackColor] set];
      [circle stroke];

      fraction = ([self floatValue] - [self minValue]) /
	([self maxValue] - [self minValue]);
      angle = (fraction * (2.0 * M_PI)) - (M_PI / 2.0);
      radius = (knobRect.size.height / 2) - 4;
      point = NSMakePoint((radius * cos(angle)) + knobCenter.x,
			  (radius * sin(angle)) + knobCenter.y);
          
      [[NSBezierPath bezierPathWithOvalInRect: NSMakeRect(point.x - 2,
							  point.y - 2,
							  4,
							  4)] stroke];
    }
  else if (_type == NSLinearSlider)
    {
      BOOL vertical = (cellFrame.size.height > cellFrame.size.width);
      NSImage *image;
      NSSize size;

      if (vertical != _isVertical)
	{
	  if (vertical == YES)
	    {
	      image = [NSImage imageNamed: @"common_SliderVert"];
	      if (image != nil)
		{
		  size = [image size];
		  [image setScalesWhenResized: YES];
		  [image setSize: NSMakeSize(cellFrame.size.width, size.height)];
		}
	    }
	  else
	    {
	      image = [NSImage imageNamed: @"common_SliderHoriz"];
	      if (image != nil)
		{
		  size = [image size];
		  [image setScalesWhenResized: YES];
		  [image setSize: NSMakeSize(size.width, cellFrame.size.height)];
		}
	    }
	  [_knobCell setImage: image];
	}
      _isVertical = vertical;

      [self drawBarInside: cellFrame flipped: [controlView isFlipped]];

      /* Draw title - Uhmmm - shouldn't this better go into
	 drawBarInside:flipped: ? */
      if (_isVertical == NO)
	{
	  [_titleCell drawInteriorWithFrame: cellFrame inView: controlView];
	}

      [self drawKnob];
    }
}

- (BOOL) isOpaque
{
  return NO;
}

/**<p> Returns the thickness of the slider's knob.  This value is in
  pixels, and is the size of the knob along the slider's track.</p>
  <p>See Also: -setKnobThickness:</p>
*/
- (float) knobThickness
{
  NSImage *image = [_knobCell image];
  NSSize size;

  if (image != nil)
    {
      size = [image size];
    }
  else 
    {
      return 0;
    }

  return _isVertical ? size.height : size.width;
}

/**<p>Sets the thickness of the knob to <var>thickness</var>, in pixels.
  This value sets the amount of space which the knob takes up in the
  slider's track.</p><p>See Also: -knobThickness</p> 
 */
- (void) setKnobThickness: (float)thickness
{
  NSImage *image = [_knobCell image];
  NSSize size;

  if (image != nil)
    {
      size = [image size];
    }
  else 
    {
      return;
    }

  if (_isVertical == YES)
    size.height = thickness;
  else
    size.width = thickness;

  [image setSize: size];

  if ((_control_view != nil) &&  
      ([_control_view isKindOfClass: [NSControl class]]))
    {
      [(NSControl*)_control_view updateCell: self];
    }
}

/**<p> Sets the value by which the slider will be be incremented when with the
    ALT key down to <var>increment</var>.</p>
    <p>See Also: -altIncrementValue</p> 
*/
- (void) setAltIncrementValue: (double)increment
{
  _altIncrementValue = increment;
}

/**<p> Sets the minimum value that the sliders represents to
   <var>maxValue</var>.</p><p>See Also: -minValue</p>
*/
- (void) setMinValue: (double)aDouble
{
  _minValue = aDouble;
  if ([self doubleValue] < _minValue)
    [self setDoubleValue: _minValue];
}

/** <p>Sets the maximum value that the sliders represents to 
    <var>maxValue</var>.</p><p>See Also: -maxValue</p>
*/
- (void) setMaxValue: (double)aDouble
{
  _maxValue = aDouble;
  if ([self doubleValue] > _maxValue)
    [self setDoubleValue: _maxValue];
}

- (void) setObjectValue: (id)anObject
{
  // If the provided object doesn't respond to doubeValue, or our minValue
  // is greater than our maxValue, we set our value to our minValue
  // (this arbitrary choice matches OS X)
  if ([anObject respondsToSelector: @selector(doubleValue)] == NO ||
      _minValue > _maxValue)
    [super setObjectValue: [NSNumber numberWithDouble: _minValue]];
  else
    {
      double aDouble = [anObject doubleValue];
      if (aDouble < _minValue)
        [super setObjectValue: [NSNumber numberWithDouble: _minValue]];
      else if (aDouble > _maxValue)
        [super setObjectValue: [NSNumber numberWithDouble: _maxValue]];
      else
        [super setObjectValue: anObject];
    }
}

/**<p>Returns the cell used to draw the title.</p>
   <p>See Also: -setTitleCell:</p> */
- (id) titleCell
{
  return _titleCell;
}

/**<p>Returns the colour used to draw the title.</p>
   <p>See Also: -setTitleColor:</p> 
*/
- (NSColor*) titleColor
{
  return [_titleCell textColor];
}

/**<p>Returns the font used to draw the title.</p>
   <p>See Also: -setTitleFont:</p>
*/
- (NSFont*) titleFont
{
  return [_titleCell font];
}

/**<p>Sets the title of the slider to <var>barTitle</var>.  This title is
  displayed on the slider's track, behind the knob.</p>
  <p>See Also: -title</p>*/
- (void) setTitle: (NSString*)title
{
  [_titleCell setStringValue: title];
}

/**<p>Returns the title of the slider. This title is
  displayed on the slider's track, behind the knob.</p>
  <p>See Also: -setTitle:</p>
*/
- (NSString*) title
{
  return [_titleCell stringValue];
}

/**<p>Sets the cell used to draw the title to <var>titleCell</var>.</p>
   <p>See Also: -titleCell</p>*/
- (void) setTitleCell: (NSCell*)aCell
{
  ASSIGN(_titleCell, aCell);
}

/**<p>Sets the colour with which the title will be drawn to <var>color</var>.
   </p><p>See Also: -titleColor</p>
*/
- (void) setTitleColor: (NSColor*)color
{
  [_titleCell setTextColor: color];
}

/**<p> Sets the font with which the title will be drawm to <var>font</var>.
 </p><p>See Also: -titleFont</p>
*/
- (void) setTitleFont: (NSFont*)font
{
  [_titleCell setFont: font];
}

/**<p>Returns the slider type: linear or circular.</p>
   <p>See Also: -setSliderType:</p>
*/
- (NSSliderType)sliderType
{
  return _type;
}

/**<p> Sets the type of the slider: linear or circular.
 </p><p>See Also: -sliderType</p>
*/
- (void) setSliderType: (NSSliderType)type
{
  _type = type;
  if (_type == NSLinearSlider)
    {
      [self setBordered: YES];
      [self setBezeled: NO];
    }
  else if (_type == NSCircularSlider)
    {
      [self setBordered: NO];
      [self setBezeled: NO];
    }
}

/**Returns whether or not the slider is vertical.  If, for some
  reason, this cannot be determined, for such reasons as the slider is
  not yet displayed, this method returns -1.  Generally, a slider is
  considered vertical if its height is greater than its width. 
*/
- (int) isVertical
{
  return _isVertical;
}

/**<p>Returns the value by which the slider is incremented when the user
    holds down the ALT key.</p><p>See Also: -setAltIncrementValue:</p>  
*/
- (double) altIncrementValue
{
  return _altIncrementValue;
}

/** 
  <p>The default implementation returns <code>YES</code>, so that the
  slider continues to track the user's movement even if the cursor
  leaves the slider's track.</p>

  <p>Do not call this method directly.  Override it in subclasses
  where the tracking behaviour needs to be different.</p>
 */
+ (BOOL) prefersTrackingUntilMouseUp
{
  return YES;
}

/** Returns the rect of the track, minus the bezel. */
- (NSRect) trackRect
{
  return _trackRect;
}

/** <p>Returns the minimum value that the slider represents.</p>
    <p>See Also: -setMinValue:</p>
*/
- (double) minValue
{
  return _minValue;
}

/**<p>Returns the maximum value that the slider represents.</p>
   <p>See Also: -setMaxValue:</p>
*/
- (double) maxValue
{
  return _maxValue;
}

// ticks
- (BOOL) allowsTickMarkValuesOnly
{
  return _allowsTickMarkValuesOnly;
}

/* verified on Cocoa that a circular slider with one tick has two values: 0, 50 */
- (double) closestTickMarkValueToValue: (double)aValue
{
  double d, f;
  int effectiveTicks;

  if (_numberOfTickMarks == 0)
    return aValue;
  
  effectiveTicks = _numberOfTickMarks;
  if (_type == NSCircularSlider)
    effectiveTicks++;

  if (effectiveTicks == 1)
    return (_maxValue + _minValue) / 2;

  if (aValue < _minValue)
    {
      aValue = _minValue;
    }
  else if (aValue > _maxValue)
    {
      aValue = _maxValue; 
    }

  d = _maxValue - _minValue;
  f = ((aValue - _minValue)  * (effectiveTicks - 1)) / d;
  f = ((GSRoundTowardsInfinity(f) * d) / (effectiveTicks - 1)) + _minValue;

  /* never return the maximum value, tested on Apple */
  if (_type == NSCircularSlider && (f >= _maxValue))
    f = _minValue;

  return f;
}

- (NSInteger) indexOfTickMarkAtPoint: (NSPoint)point
{
  NSInteger i;

  for (i = 0; i < _numberOfTickMarks; i++)
    {
      if (NSPointInRect(point, [self rectOfTickMarkAtIndex: i])) 
        {
	  return i;
	}
    }

  return NSNotFound;
}

- (NSInteger) numberOfTickMarks
{
  return _numberOfTickMarks;
}

- (NSRect) rectOfTickMarkAtIndex: (NSInteger)index
{
  NSRect rect = _trackRect;
  float d;

  if ((index < 0) || (index >= _numberOfTickMarks))
    {
      [NSException raise: NSRangeException
		   format: @"Index of tick mark out of bounds."];
    }

  if (_numberOfTickMarks > 1)
    {
      if (_isVertical)
	{
	  d = NSHeight(rect) / (_numberOfTickMarks - 1);
	  rect.size.height = d;
	  rect.origin.y += d * index;
	}
      else
	{
	  d = NSWidth(rect) / (_numberOfTickMarks - 1);
	  rect.size.width = d;
	  rect.origin.x += d * index;
	}
    }

  return rect;
}

- (void) setAllowsTickMarkValuesOnly: (BOOL)flag
{
  _allowsTickMarkValuesOnly = flag;
}

- (void) setNumberOfTickMarks: (NSInteger)numberOfTickMarks
{
  _numberOfTickMarks = numberOfTickMarks;
  if ((_control_view != nil) &&  
      ([_control_view isKindOfClass: [NSControl class]]))
    {
      [(NSControl*)_control_view updateCell: self];
    }
}

- (void) setTickMarkPosition: (NSTickMarkPosition)position
{
  _tickMarkPosition = position;
  if ((_control_view != nil) &&  
      ([_control_view isKindOfClass: [NSControl class]]))
    {
      [(NSControl*)_control_view updateCell: self];
    }
}

- (NSTickMarkPosition) tickMarkPosition
{
  return _tickMarkPosition;
}

- (double) tickMarkValueAtIndex: (NSInteger)index
{
  if ((index < 0) || (index >= _numberOfTickMarks))
    {
      [NSException raise: NSRangeException
		   format: @"Index of tick mark out of bounds."];
    }

  if (_numberOfTickMarks == 1)
    return (_maxValue + _minValue) / 2;
  if (index >= _numberOfTickMarks)
    return _maxValue;
  if (index <= 0)
    return _minValue;

  if (_type == NSCircularSlider)
    return _minValue + index * (_maxValue - _minValue) / _numberOfTickMarks;
  if (_type == NSLinearSlider)
    return _minValue + index * (_maxValue - _minValue) / (_numberOfTickMarks - 1);

  return 0.0;
}

- (BOOL) trackMouse: (NSEvent*)theEvent
	     inRect: (NSRect)cellFrame
	     ofView: (NSView*)controlView
       untilMouseUp: (BOOL)flag
{
  float delay;
  float interval;
  id target = [self target];
  SEL action = [self action];
  unsigned int eventMask = NSLeftMouseDownMask | NSLeftMouseUpMask
                           | NSLeftMouseDraggedMask | NSMouseMovedMask;
  NSEventType eventType = [theEvent type];
  BOOL isContinuous = [self isContinuous];
  float oldFloatValue = [self floatValue];
  NSRect slotRect = [self trackRect];
  BOOL isVertical = [self isVertical];
  float minValue = [self minValue];
  float maxValue = [self maxValue];
  BOOL isFlipped = [controlView isFlipped];
  NSPoint location = [theEvent locationInWindow];
  NSPoint point = [controlView convertPoint: location fromView: nil];
  NSRect knobRect = [self knobRectFlipped: isFlipped];

  _mouse_down_flags = [theEvent modifierFlags];
  if (![self isEnabled])
    {
      return NO;
    }

  if (![controlView mouse: point inRect: knobRect])
    {
      // Mouse is not on the knob, move the knob to the mouse position
      float floatValue;

      floatValue = _floatValueForMousePoint(point, knobRect, 
					    slotRect, isVertical, 
					    minValue, maxValue,
					    self, isFlipped,
					    (_type == NSCircularSlider)); 
      [self setFloatValue: floatValue];
      if (isContinuous)
        {
	  [(NSControl*)controlView sendAction: action to: target];
	}
    }
      
  if (isContinuous)
    {
      [self getPeriodicDelay: &delay interval: &interval];
      [NSEvent startPeriodicEventsAfterDelay: delay withPeriod: interval];
      eventMask |= NSPeriodicMask;
    }

  while (eventType != NSLeftMouseUp)
    {
      theEvent = [NSApp nextEventMatchingMask: eventMask
			untilDate: [NSDate distantFuture]
			inMode: NSEventTrackingRunLoopMode
			dequeue: YES];
      eventType = [theEvent type];

      if (eventType == NSPeriodic)
        {
	  NSWindow *w = [controlView window];

	  location = [w mouseLocationOutsideOfEventStream];
	}
      else
        {
	  location = [theEvent locationInWindow];
	}
      point = [controlView convertPoint: location fromView: nil];

      if (point.x != knobRect.origin.x || point.y != knobRect.origin.y)
        {
	  float floatValue;

	  floatValue = _floatValueForMousePoint(point, knobRect,
						slotRect, isVertical, 
						minValue, maxValue, 
						self, isFlipped,
						(_type == NSCircularSlider)); 
	  if (floatValue != oldFloatValue)
	    {
	      if (_allowsTickMarkValuesOnly)
		{
		  floatValue = [self closestTickMarkValueToValue:floatValue]; 
		}

	      [self setFloatValue: floatValue];
	      if (isContinuous)
	        {
		  [(NSControl*)controlView sendAction: action to: target];
		}
	      oldFloatValue = floatValue;
	    }
	  knobRect.origin = point;
	}
    }

  // If the cell is not continuous send the action at the end of the drag
  if (!isContinuous)
    {
      [(NSControl*)controlView sendAction: action to: target];
    }
  else
    {
      [NSEvent stopPeriodicEvents];
    }

  return YES;
}

- (id) initWithCoder: (NSCoder*)decoder
{
  self = [super initWithCoder: decoder];
  if (self == nil)
    return nil;

  if ([decoder allowsKeyedCoding])
    {
      _allowsTickMarkValuesOnly = [decoder decodeBoolForKey: @"NSAllowsTickMarkValuesOnly"];
      _numberOfTickMarks = [decoder decodeIntForKey: @"NSNumberOfTickMarks"];
      _tickMarkPosition = [decoder decodeIntForKey: @"NSTickMarkPosition"];
      [self setMinValue: [decoder decodeFloatForKey: @"NSMinValue"]];
      [self setMaxValue: [decoder decodeFloatForKey: @"NSMaxValue"]];
      [self setFloatValue: [decoder decodeFloatForKey: @"NSValue"]];
      _altIncrementValue = [decoder decodeFloatForKey: @"NSAltIncValue"];
      [self setSliderType: [decoder decodeIntForKey: @"NSSliderType"]];
	  
      // do these here, since the Cocoa version of the class does not save these values...
      _knobCell = [NSCell new];
      _titleCell = [NSTextFieldCell new];
      [_titleCell setTextColor: [NSColor controlTextColor]];
      [_titleCell setStringValue: @""];
      [_titleCell setAlignment: NSCenterTextAlignment];

      _isVertical = -1;
    }
  else
    {
      float minValue, maxValue;
      [decoder decodeValuesOfObjCTypes: "fffi",
	       &minValue, &maxValue, &_altIncrementValue, &_isVertical];
      [self setMinValue: minValue];
      [self setMaxValue: maxValue];
      [decoder decodeValueOfObjCType: @encode(id) at: &_titleCell];
      [decoder decodeValueOfObjCType: @encode(id) at: &_knobCell];
      if ([decoder versionForClassName: @"NSSliderCell"] >= 2)
	{
	  [decoder decodeValueOfObjCType: @encode(BOOL) at: &_allowsTickMarkValuesOnly];
	  [decoder decodeValueOfObjCType: @encode(int) at: &_numberOfTickMarks];
	  [decoder decodeValueOfObjCType: @encode(int) at: &_tickMarkPosition];
	}
    }
  return self;
}

- (void) encodeWithCoder: (NSCoder*)coder
{
  [super encodeWithCoder: coder];
  if ([coder allowsKeyedCoding])
    {
      [coder encodeBool: _allowsTickMarkValuesOnly forKey: @"NSAllowsTickMarkValuesOnly"];
      [coder encodeInt: _numberOfTickMarks forKey: @"NSNumberOfTickMarks"];
      [coder encodeInt: _tickMarkPosition forKey: @"NSTickMarkPosition"];
      [coder encodeFloat: _minValue forKey: @"NSMinValue"];
      [coder encodeFloat: _maxValue forKey: @"NSMaxValue"];
      [coder encodeFloat: _altIncrementValue forKey: @"NSAltIncValue"];
      [coder encodeFloat: _minValue forKey: @"NSValue"]; // encoded for compatibility
      [coder encodeInt: _type forKey: @"NSSliderType"];
    }
  else
    {
      [coder encodeValuesOfObjCTypes: "fffi",
	     &_minValue, &_maxValue, &_altIncrementValue, &_isVertical];
      [coder encodeValueOfObjCType: @encode(id) at: &_titleCell];
      [coder encodeValueOfObjCType: @encode(id) at: &_knobCell];
      // New for version 2
      [coder encodeValueOfObjCType: @encode(BOOL) at: &_allowsTickMarkValuesOnly];
      [coder encodeValueOfObjCType: @encode(int) at: &_numberOfTickMarks];
      [coder encodeValueOfObjCType: @encode(int) at: &_tickMarkPosition];
    }
}
  
@end
