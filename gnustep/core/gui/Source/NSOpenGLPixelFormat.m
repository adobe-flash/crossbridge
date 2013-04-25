/** <title>NSOpenGLPixelFormat.m </title>

   <abstract>use to choose the type of GL context</abstract>

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author:  Frederic De Jaeger
   Date: 2002

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

#import <Foundation/NSString.h>
#import <Foundation/NSDebug.h>
#import "AppKit/NSOpenGL.h"
#import "GNUstepGUI/GSDisplayServer.h"


// @interface GSGLPixelFormat : NSOpenGLPixelFormat
// {}
// + _classContext;
// @end

// @implementation GSGLPixelFormat
// - (id)initWithAttributes:(NSOpenGLPixelFormatAttribute *)attribs
// {
//   self = [[GSGLPixelFormat _classContext] alloc];

//   return [self initWithAttributes: attribs];
// }
// @end

//static GSGLPixelFormat *temp;

@implementation NSOpenGLPixelFormat
+ _classPixelFormat
{
  Class glPixelFormatClass = [GSCurrentServer() glPixelFormatClass];

  if (glPixelFormatClass == nil)
    {
      NSWarnMLog(@"Backend doesn't have any glPixelFormatClass");
      return nil;
    }
  else
    {
      NSDebugMLLog(@"GLX", @"found a class %@", glPixelFormatClass);
      return glPixelFormatClass;
    }
}

// + (void) initialize
// {
//   if (self == [NSOpenGLPixelFormat class])
//     {
//       temp = (GSGLPixelFormat *) NSAllocateObject([GSGLPixelFormat class], 0, 
// 						  NSDefaultMallocZone());
//     }
// }

+ allocWithZone: (NSZone *) z
{
  Class c = [self _classPixelFormat];
  if (c)
    return NSAllocateObject(c, 0, z);
  else
    return nil;
}

- (void)getValues:(long *)vals 
     forAttribute:(NSOpenGLPixelFormatAttribute)attrib 
 forVirtualScreen:(int)screen
{
  [self subclassResponsibility: _cmd];
}

- (id)initWithAttributes:(NSOpenGLPixelFormatAttribute *)attribs
{
  [self subclassResponsibility: _cmd];
  return nil;
}

- (int)numberOfVirtualScreens
{
  [self subclassResponsibility: _cmd];
  return 0;
}

@end



