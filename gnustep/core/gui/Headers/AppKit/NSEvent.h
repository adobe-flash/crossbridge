/* 
   NSEvent.h

   The event class

   Copyright (C) 1996,1999 Free Software Foundation, Inc.

   Author:  Scott Christley <scottc@net-community.com>
   Date: 1996
   
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

#ifndef _GNUstep_H_NSEvent
#define _GNUstep_H_NSEvent
#import <GNUstepBase/GSVersionMacros.h>

#import <Foundation/NSObject.h>
#import <Foundation/NSGeometry.h>
// For NSTimeInterval
#import <Foundation/NSDate.h>

@class NSString;
@class NSWindow;
@class NSGraphicsContext;

/**
 * Enumeration of event types recognized within GNUstep GUI.  Each type has a
 * corresponding mask that can be used when filtering for multiple types.  For
 * example, the <code>NSLeftMouseDown</code> type has
 * <code>NSLeftMouseDownMask</code> for its mask.  The special mask
 * <code>NSAnyEventMask</code> matches any event.  The complete list of types
 * is as follows:
 * <example>
  NSLeftMouseDown,
  NSLeftMouseUp,
  NSOtherMouseDown,
  NSOtherMouseUp,
  NSRightMouseDown,
  NSRightMouseUp,
  NSMouseMoved,
  NSLeftMouseDragged,
  NSOtherMouseDragged,
  NSRightMouseDragged,
  NSMouseEntered,
  NSMouseExited,
  NSKeyDown,
  NSKeyUp,
  NSFlagsChanged,
  NSAppKitDefined,       // reserved
  NSSystemDefined,       // reserved
  NSApplicationDefined,  // available for custom use by apps
  NSPeriodic,
  NSCursorUpdate,
  NSScrollWheel
 </example>
 */
typedef enum _NSEventType {
  // Note - order IS significant as ranges of values
  // are used for testing for valid event types.
  NSLeftMouseDown = 1,
  NSLeftMouseUp,
  NSRightMouseDown,
  NSRightMouseUp,
  NSMouseMoved,
  NSLeftMouseDragged,
  NSRightMouseDragged,
  NSMouseEntered,
  NSMouseExited,
  NSKeyDown,
  NSKeyUp,
  NSFlagsChanged,
  NSAppKitDefined,
  NSSystemDefined,
  NSApplicationDefined,
  NSPeriodic,
  NSCursorUpdate,
  NSScrollWheel = 22,
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
  NSTabletPoint,
  NSTabletProximity,
#endif
  NSOtherMouseDown = 25,
  NSOtherMouseUp,
  NSOtherMouseDragged
} NSEventType;

enum {
  NSLeftMouseDownMask = (1 << NSLeftMouseDown),
  NSLeftMouseUpMask = (1 << NSLeftMouseUp),
  NSRightMouseDownMask = (1 << NSRightMouseDown),
  NSRightMouseUpMask = (1 << NSRightMouseUp),
  NSMouseMovedMask = (1 << NSMouseMoved),
  NSLeftMouseDraggedMask = (1 << NSLeftMouseDragged),
  NSRightMouseDraggedMask = (1 << NSRightMouseDragged),
  NSMouseEnteredMask = (1 << NSMouseEntered),
  NSMouseExitedMask = (1 << NSMouseExited),
  NSKeyDownMask = (1 << NSKeyDown),
  NSKeyUpMask = (1 << NSKeyUp),
  NSFlagsChangedMask = (1 << NSFlagsChanged),
  NSAppKitDefinedMask = (1 << NSAppKitDefined),
  NSSystemDefinedMask = (1 << NSSystemDefined),
  NSApplicationDefinedMask = (1 << NSApplicationDefined),
  NSPeriodicMask = (1 << NSPeriodic),
  NSCursorUpdateMask = (1 << NSCursorUpdate),
  NSScrollWheelMask = (1 << NSScrollWheel),
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
  NSTabletPointMask = (1 << NSTabletPoint),
  NSTabletProximityMask = (1 << NSTabletProximity),
#endif
  NSOtherMouseDownMask = (1 << NSOtherMouseDown),
  NSOtherMouseUpMask = (1 << NSOtherMouseUp),
  NSOtherMouseDraggedMask = (1 << NSOtherMouseDragged),

  NSAnyEventMask = 0xffffffffU,

  // key events
  GSKeyEventMask = (NSKeyDownMask | NSKeyUpMask | NSFlagsChangedMask),
  // mouse events
  GSMouseEventMask = (NSLeftMouseDownMask | NSLeftMouseUpMask | NSLeftMouseDraggedMask
                      | NSRightMouseDownMask | NSRightMouseUpMask | NSRightMouseDraggedMask
                      | NSOtherMouseDownMask | NSOtherMouseUpMask | NSOtherMouseDraggedMask
                      | NSMouseMovedMask | NSScrollWheelMask
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
                      | NSTabletPointMask | NSTabletProximityMask
#endif
      ),
  // mouse move events
  GSMouseMovedEventMask = (NSMouseMovedMask | NSScrollWheelMask
                           | NSLeftMouseDraggedMask | NSRightMouseDraggedMask
                           | NSOtherMouseDraggedMask),
  // enter/exit event
  GSEnterExitEventMask = (NSMouseEnteredMask | NSMouseExitedMask | NSCursorUpdateMask),
  // other events
  GSOtherEventMask = (NSAppKitDefinedMask | NSSystemDefinedMask
                      | NSApplicationDefinedMask | NSPeriodicMask),

  // tracking loops may need to add NSPeriodicMask
  GSTrackingLoopMask = (NSLeftMouseDownMask | NSLeftMouseUpMask
                        | NSLeftMouseDraggedMask | NSMouseMovedMask
                        | NSRightMouseUpMask | NSOtherMouseUpMask)
};

/*
 * Convert an NSEvent Type to it's respective Event Mask
 */
// FIXME: Should we use the inline trick from NSGeometry.h here? 
static inline
unsigned int
NSEventMaskFromType(NSEventType type)
{
  return (1 << type);
}

enum {
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
  NSDeviceIndependentModifierFlagsMask = 0xffff0000U,
#endif
  NSAlphaShiftKeyMask = 1 << 16,
  NSShiftKeyMask = 2 << 16,
  NSControlKeyMask = 4 << 16,
  NSAlternateKeyMask = 8 << 16,
  NSCommandKeyMask = 16 << 16,
  NSNumericPadKeyMask = 32 << 16,
  NSHelpKeyMask = 64 << 16,
  NSFunctionKeyMask = 128 << 16
};

#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
typedef enum
{
  NSUnknownPointingDevice,
  NSPenPointingDevice,
  NSCursorPointingDevice,
  NSEraserPointingDevice
} NSPointingDeviceType;

enum
{
  NSPenTipMask = 1,
  NSPenLowerSideMask = 2,
  NSPenUpperSideMask = 4
};

enum
{
  NSMouseEventSubtype,
  NSTabletPointEventSubtype,
  NSTabletProximityEventSubtype
};

enum {
  NSWindowExposedEventType = 0,
  NSApplicationActivatedEventType = 1,
  NSApplicationDeactivatedEventType = 2,
  NSWindowMovedEventType = 4,
  NSScreenChangedEventType = 8,
  NSAWTEventType = 16
};

enum
{
  NSPowerOffEventType = 1
};

#endif

@interface NSEvent : NSObject <NSCoding, NSCopying>
{
  NSEventType	event_type;
  NSPoint	location_point;
  unsigned int modifier_flags;
  NSTimeInterval event_time;
  int window_num;
  NSGraphicsContext	*event_context;
  union _MB_event_data
    {
      struct
        {
          int   event_num;
          short click;
          short button;
          float pressure;
          float deltaX;
          float deltaY;
          float deltaZ;
        } mouse;
      struct
        {
          BOOL     repeat;
          __unsafe_unretained NSString *char_keys;
          __unsafe_unretained NSString *unmodified_keys;
          unsigned short key_code;
        } key;
      struct
        {
          int  event_num;
          int  tracking_num;
          void *user_data;
        } tracking;
      struct
        {
          short sub_type;
          int   data1;
          int   data2;
        } misc;
    } event_data;
}

+ (NSEvent*) enterExitEventWithType: (NSEventType)type        
                           location: (NSPoint)location
                      modifierFlags: (unsigned int)flags
                          timestamp: (NSTimeInterval)time
                       windowNumber: (int)windowNum
                            context: (NSGraphicsContext*)context        
                        eventNumber: (int)eventNum
                     trackingNumber: (int)trackingNum
                           userData: (void *)userData; 

+ (NSEvent*) keyEventWithType: (NSEventType)type
                     location: (NSPoint)location
                modifierFlags: (unsigned int)flags
                    timestamp: (NSTimeInterval)time
                 windowNumber: (int)windowNum
                      context: (NSGraphicsContext*)context        
                   characters: (NSString *)keys        
  charactersIgnoringModifiers: (NSString *)ukeys
                    isARepeat: (BOOL)repeatKey        
                      keyCode: (unsigned short)code;

+ (NSEvent*) mouseEventWithType: (NSEventType)type        
                       location: (NSPoint)location
                  modifierFlags: (unsigned int)flags
                      timestamp: (NSTimeInterval)time
                   windowNumber: (int)windowNum        
                        context: (NSGraphicsContext*)context        
                    eventNumber: (int)eventNum        
                     clickCount: (int)clickNum        
                       pressure: (float)pressureValue;

#if OS_API_VERSION(GS_API_NONE, GS_API_NONE)
+ (NSEvent*) mouseEventWithType: (NSEventType)type        
                       location: (NSPoint)location
                  modifierFlags: (unsigned int)flags
                      timestamp: (NSTimeInterval)time
                   windowNumber: (int)windowNum        
                        context: (NSGraphicsContext*)context        
                    eventNumber: (int)eventNum        
                     clickCount: (int)clickNum        
                       pressure: (float)pressureValue
                   buttonNumber: (int)buttonNum
                         deltaX: (float)deltaX
                         deltaY: (float)deltaY
                         deltaZ: (float)deltaZ;
#endif

+ (NSPoint)mouseLocation;

+ (NSEvent*) otherEventWithType: (NSEventType)type
                       location: (NSPoint)location
                  modifierFlags: (unsigned int)flags
                      timestamp: (NSTimeInterval)time
                   windowNumber: (int)windowNum
                        context: (NSGraphicsContext*)context
                        subtype: (short)subType
                          data1: (int)data1
                          data2: (int)data2;

+ (void) startPeriodicEventsAfterDelay: (NSTimeInterval)delaySeconds
                            withPeriod: (NSTimeInterval)periodSeconds;
+ (void) stopPeriodicEvents;


#if OS_API_VERSION(GS_API_MACOSX, GS_API_LATEST)
- (int) buttonNumber;
#endif
- (NSString *) characters;
- (NSString *) charactersIgnoringModifiers;
- (int) clickCount;
- (NSGraphicsContext*) context;
- (int) data1;
- (int) data2;
#if OS_API_VERSION(GS_API_MACOSX, GS_API_LATEST)
- (float)deltaX;
- (float)deltaY;
- (float)deltaZ;
#endif
- (int) eventNumber;
- (BOOL) isARepeat;
- (unsigned short) keyCode;
- (NSPoint) locationInWindow;
- (unsigned int) modifierFlags;
- (float) pressure;
- (short) subtype;
- (NSTimeInterval) timestamp;
- (int) trackingNumber;
- (NSEventType) type;
- (void *) userData;
- (NSWindow *) window;
- (int) windowNumber;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
- (int) absoluteX;
- (int) absoluteY;
- (int) absoluteZ;
- (unsigned int) buttonMask;
- (unsigned int) capabilityMask;
- (unsigned int) deviceID;
- (BOOL) isEnteringProximity;
- (unsigned int) pointingDeviceID;
- (unsigned int) pointingDeviceSerialNumber;
- (NSPointingDeviceType) pointingDeviceType;
- (float) rotation;
- (unsigned int) systemTabletID;
- (unsigned int) tabletID;
- (float) tangentialPressure;
- (NSPoint) tilt;
- (unsigned long long) uniqueID;
- (id) vendorDefined;
- (unsigned int) vendorID;
- (unsigned int) vendorPointingDeviceType;
#endif

@end

enum {
  NSUpArrowFunctionKey = 0xF700,
  NSDownArrowFunctionKey = 0xF701,
  NSLeftArrowFunctionKey = 0xF702,
  NSRightArrowFunctionKey = 0xF703,
  NSF1FunctionKey  = 0xF704,
  NSF2FunctionKey  = 0xF705,
  NSF3FunctionKey  = 0xF706,
  NSF4FunctionKey  = 0xF707,
  NSF5FunctionKey  = 0xF708,
  NSF6FunctionKey  = 0xF709,
  NSF7FunctionKey  = 0xF70A,
  NSF8FunctionKey  = 0xF70B,
  NSF9FunctionKey  = 0xF70C,
  NSF10FunctionKey = 0xF70D,
  NSF11FunctionKey = 0xF70E,
  NSF12FunctionKey = 0xF70F,
  NSF13FunctionKey = 0xF710,
  NSF14FunctionKey = 0xF711,
  NSF15FunctionKey = 0xF712,
  NSF16FunctionKey = 0xF713,
  NSF17FunctionKey = 0xF714,
  NSF18FunctionKey = 0xF715,
  NSF19FunctionKey = 0xF716,
  NSF20FunctionKey = 0xF717,
  NSF21FunctionKey = 0xF718,
  NSF22FunctionKey = 0xF719,
  NSF23FunctionKey = 0xF71A,
  NSF24FunctionKey = 0xF71B,
  NSF25FunctionKey = 0xF71C,
  NSF26FunctionKey = 0xF71D,
  NSF27FunctionKey = 0xF71E,
  NSF28FunctionKey = 0xF71F,
  NSF29FunctionKey = 0xF720,
  NSF30FunctionKey = 0xF721,
  NSF31FunctionKey = 0xF722,
  NSF32FunctionKey = 0xF723,
  NSF33FunctionKey = 0xF724,
  NSF34FunctionKey = 0xF725,
  NSF35FunctionKey = 0xF726,
  NSInsertFunctionKey = 0xF727,
  NSDeleteFunctionKey = 0xF728,
  NSHomeFunctionKey = 0xF729,
  NSBeginFunctionKey = 0xF72A,
  NSEndFunctionKey = 0xF72B,
  NSPageUpFunctionKey = 0xF72C,
  NSPageDownFunctionKey = 0xF72D,
  NSPrintScreenFunctionKey = 0xF72E,
  NSScrollLockFunctionKey = 0xF72F,
  NSPauseFunctionKey = 0xF730,
  NSSysReqFunctionKey = 0xF731,
  NSBreakFunctionKey = 0xF732,
  NSResetFunctionKey = 0xF733,
  NSStopFunctionKey = 0xF734,
  NSMenuFunctionKey = 0xF735,
  NSUserFunctionKey = 0xF736,
  NSSystemFunctionKey = 0xF737,
  NSPrintFunctionKey = 0xF738,
  NSClearLineFunctionKey = 0xF739,
  NSClearDisplayFunctionKey = 0xF73A,
  NSInsertLineFunctionKey = 0xF73B,
  NSDeleteLineFunctionKey = 0xF73C,
  NSInsertCharFunctionKey = 0xF73D,
  NSDeleteCharFunctionKey = 0xF73E,
  NSPrevFunctionKey = 0xF73F,
  NSNextFunctionKey = 0xF740,
  NSSelectFunctionKey = 0xF741,
  NSExecuteFunctionKey = 0xF742,
  NSUndoFunctionKey = 0xF743,
  NSRedoFunctionKey = 0xF744,
  NSFindFunctionKey = 0xF745,
  NSHelpFunctionKey = 0xF746,
  NSModeSwitchFunctionKey = 0xF747
};

/*
 * Convert an Event Mask Type to a Mask
 */
unsigned int NSEventMaskFromType(NSEventType type);

#if OS_API_VERSION(GS_API_NONE, GS_API_NONE)
typedef enum {
  GSAppKitWindowMoved = 1,
  GSAppKitWindowResized,
  GSAppKitWindowClose,
  GSAppKitWindowMiniaturize,
  GSAppKitWindowFocusIn,
  GSAppKitWindowFocusOut,
  GSAppKitWindowLeave,
  GSAppKitWindowEnter,
  GSAppKitDraggingEnter,
  GSAppKitDraggingUpdate,
  GSAppKitDraggingStatus,
  GSAppKitDraggingExit,
  GSAppKitDraggingDrop,
  GSAppKitDraggingFinished,
  GSAppKitRegionExposed,
  GSAppKitWindowDeminiaturize,
} GSAppKitSubtype;
#endif

#endif /* _GNUstep_H_NSEvent */
