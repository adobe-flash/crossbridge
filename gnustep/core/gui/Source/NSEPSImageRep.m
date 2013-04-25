/** <title>NSEPSImageRep</title>

   <abstract>EPS image representation.</abstract>

   Copyright (C) 1996 Free Software Foundation, Inc.
   
   Author:  Adam Fedor <fedor@colorado.edu>
   Date: Feb 1996
   
   This file is part of the GNUstep Application Kit Library.

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

#import <Foundation/NSArray.h>
#import <Foundation/NSCoder.h>
#import <Foundation/NSData.h>
#import "AppKit/NSPasteboard.h"
#import "AppKit/NSEPSImageRep.h"

@implementation NSEPSImageRep 

+ (BOOL) canInitWithData: (NSData *)data
{
  char buffer[2];

  [data getBytes: buffer length: 2];

  // Simple check for Postscript
  if (buffer[0] == '%' && buffer[1] == '!')
    return YES;
  else
    return NO;
}

+ (NSArray *) imageUnfilteredFileTypes
{
  static NSArray *types = nil;

  if (types == nil)
    {
      types = [[NSArray alloc] initWithObjects: @"eps", nil];
    }

  return types;
}

+ (NSArray *) imageUnfilteredPasteboardTypes
{
  static NSArray *types = nil;

  if (types == nil)
    {
      types = [[NSArray alloc] initWithObjects: NSPostScriptPboardType, nil];
    }
  
  return types;
}

// Initializing a New Instance 
+ (id) imageRepWithData: (NSData *)epsData
{
  return AUTORELEASE([[self alloc] initWithData: epsData]);
}

- (id) initWithData: (NSData *)epsData
{
  [self notImplemented: _cmd];

  _epsData = epsData;
  // Set bounds from parsed header
  //_bounds = NSMakeRect();
  return self;
}

// Getting Image Data 
- (NSRect) boundingBox
{
  return _bounds;
}

- (NSData *) EPSRepresentation
{
  return _epsData;
}

- (void) prepareGState
{
  // This is for subclasses only
}

// Drawing the Image 
- (BOOL) draw
{
  [self notImplemented: _cmd];

  [self prepareGState];

  return YES;
}

// NSCopying protocol
- (id) copyWithZone: (NSZone *)zone
{
  NSEPSImageRep *copy = [super copyWithZone: zone];

  copy->_epsData = [_epsData copyWithZone: zone];

  return copy;
}

// NSCoding protocol
- (void) encodeWithCoder: (NSCoder*)aCoder
{
  NSData *data = [self EPSRepresentation];

  [super encodeWithCoder: aCoder];
  [data encodeWithCoder: aCoder];

}

- (id) initWithCoder: (NSCoder*)aDecoder
{
  NSData	*data;

  self = [super initWithCoder: aDecoder];
  data = [aDecoder decodeObject];
  return [self initWithData: data];
}

@end
