/*
   CairoContext.m

   Copyright (C) 2003 Free Software Foundation, Inc.

   August 31, 2003
   Written by Banlu Kemiyatorn <object at gmail dot com>

   This file is part of GNUstep.

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

#include <AppKit/NSBitmapImageRep.h>
#include <AppKit/NSGraphics.h>
#include <AppKit/NSPrintInfo.h>
#include <AppKit/NSPrintOperation.h>

#include "cairo/CairoContext.h"
#include "cairo/CairoGState.h"
#include "cairo/CairoSurface.h"
#include "cairo/CairoPSSurface.h"
#include "cairo/CairoPDFSurface.h"
#include "cairo/CairoFontInfo.h"
#include "cairo/CairoFontEnumerator.h"
#include "config.h"

#define CGSTATE ((CairoGState *)gstate)

#if BUILD_SERVER == SERVER_x11
#  include "x11/XGServer.h"
#  ifdef USE_GLITZ
#    define _CAIRO_SURFACE_CLASSNAME XGCairoGlitzSurface
#    include "cairo/XGCairoGlitzSurface.h"
#  else
//#    define _CAIRO_SURFACE_CLASSNAME XGCairoSurface
//#    include "cairo/XGCairoSurface.h"
//#    define _CAIRO_SURFACE_CLASSNAME XGCairoXImageSurface
//#    include "cairo/XGCairoXImageSurface.h"
#    define _CAIRO_SURFACE_CLASSNAME XGCairoModernSurface
#    include "cairo/XGCairoModernSurface.h"
#  endif /* USE_GLITZ */
#  include "x11/XGServerWindow.h"
#  include "x11/XWindowBuffer.h"
#elif BUILD_SERVER == SERVER_win32
#  include <windows.h>
#  ifdef USE_GLITZ
#    define _CAIRO_SURFACE_CLASSNAME Win32CairoGlitzSurface
#    include "cairo/Win32CairoGlitzSurface.h"
#  else
#    define _CAIRO_SURFACE_CLASSNAME Win32CairoSurface
#    include "cairo/Win32CairoSurface.h"
#  endif /* USE_GLITZ */
#else
#  error Invalid server for Cairo backend : non implemented
#endif /* BUILD_SERVER */

@interface NSBitmapImageRep (GSPrivate)
- (NSBitmapImageRep *) _convertToFormatBitsPerSample: (int)bps
                                     samplesPerPixel: (int)spp
                                            hasAlpha: (BOOL)alpha
                                            isPlanar: (BOOL)isPlanar
                                      colorSpaceName: (NSString*)colorSpaceName
                                        bitmapFormat: (NSBitmapFormat)bitmapFormat 
                                         bytesPerRow: (int)rowBytes
                                        bitsPerPixel: (int)pixelBits;
@end

@implementation CairoContext

+ (void) initializeBackend
{
  [NSGraphicsContext setDefaultContextClass: self];

  [GSFontEnumerator setDefaultClass: [CairoFontEnumerator class]];
  [GSFontInfo setDefaultClass: [CairoFontInfo class]];
}

+ (Class) GStateClass
{
  return [CairoGState class];
}

+ (BOOL) handlesPS
{
  return YES;
}

- (BOOL) supportsDrawGState
{
  return YES;
}

- (BOOL) isDrawingToScreen
{
  CairoSurface *surface = nil;
  [CGSTATE GSCurrentSurface: &surface : NULL : NULL];
  return [surface isDrawingToScreen];
}

- (void) flushGraphics
{
  // FIXME: Why is this here? When is it called?
#if BUILD_SERVER == SERVER_x11
  XFlush([(XGServer *)server xDisplay]);
#endif // BUILD_SERVER = SERVER_x11
}

#if BUILD_SERVER == SERVER_x11

/* Private backend methods */
+ (void) handleExposeRect: (NSRect)rect forDriver: (void *)driver
{
  if ([(id)driver isKindOfClass: [XWindowBuffer class]])
    {
      // For XGCairoXImageSurface
      [(XWindowBuffer *)driver _exposeRect: rect];
    }
  else if ([(id)driver isKindOfClass: [CairoSurface class]])
    {
      // For XGCairoModernSurface
      [(CairoSurface *)driver handleExposeRect: rect];
    }
}

#ifdef XSHM

+ (void) _gotShmCompletion: (Drawable)d
{
  [XWindowBuffer _gotShmCompletion: d];
}

- (void) gotShmCompletion: (Drawable)d
{
  [XWindowBuffer _gotShmCompletion: d];
}

#endif // XSHM

#endif // BUILD_SERVER = SERVER_x11

@end 

@implementation CairoContext (Ops) 

- (void) GSDrawImage: (NSRect)rect: (void *)imageref
{
  NSBitmapImageRep *bitmap;
  const unsigned char *data[5];
  NSString *colorSpaceName;

  bitmap = (NSBitmapImageRep*)imageref;
  colorSpaceName = [bitmap colorSpaceName];
  if ([bitmap isPlanar] || ([bitmap bitmapFormat] != 0) 
      || ([bitmap bitsPerSample] != 8) ||
      (![colorSpaceName isEqualToString: NSDeviceRGBColorSpace] &&
       ![colorSpaceName isEqualToString: NSCalibratedRGBColorSpace]))
    {
      int bitsPerSample = 8;
      BOOL isPlanar = NO;
      int samplesPerPixel = [bitmap hasAlpha] ? 4 : 3;
      NSString *colorSpaceName = NSCalibratedRGBColorSpace;
      NSBitmapImageRep *new;

     new = [bitmap _convertToFormatBitsPerSample: bitsPerSample
                    samplesPerPixel: samplesPerPixel
                    hasAlpha: [bitmap hasAlpha]
                    isPlanar: isPlanar
                    colorSpaceName: colorSpaceName
                    bitmapFormat: 0
                    bytesPerRow: 0
                    bitsPerPixel: 0];
            
      if (new == nil)
        {
          NSLog(@"Could not convert bitmap data");
          return;
        }
      bitmap = new;
    }

  [bitmap getBitmapDataPlanes: (unsigned char **)&data];
  [self NSDrawBitmap: rect : [bitmap pixelsWide] : [bitmap pixelsHigh]
        : [bitmap bitsPerSample] : [bitmap samplesPerPixel]
        : [bitmap bitsPerPixel] : [bitmap bytesPerRow] : [bitmap isPlanar]
        : [bitmap hasAlpha] :  [bitmap colorSpaceName]
        : data];
}

- (void) GSCurrentDevice: (void **)device : (int *)x : (int *)y
{
  CairoSurface *surface;

  [CGSTATE GSCurrentSurface: &surface : x : y];
  if (device)
    {
      *device = surface->gsDevice;
    }
}

- (void) GSSetDevice: (void *)device : (int)x : (int)y
{
  CairoSurface *surface;

  surface = [[_CAIRO_SURFACE_CLASSNAME alloc] initWithDevice: device];

  [CGSTATE GSSetSurface: surface : x : y];
  [surface release];
}

- (void) beginPrologueBBox: (NSRect)boundingBox
              creationDate: (NSString*)dateCreated
                 createdBy: (NSString*)anApplication
                     fonts: (NSString*)fontNames
                   forWhom: (NSString*)user
                     pages: (int)numPages
                     title: (NSString*)aTitle
{
  CairoSurface *surface;
  NSSize size;
  NSString *contextType;

  NSPrintOperation *printOp = [NSPrintOperation currentOperation];
  NSPrintInfo *printInfo = [printOp printInfo];

  if (printInfo != nil)
    {
      size = [printInfo paperSize];

      // FIXME: This is confusing..
      // When an 8.5x11 page is set to landscape,
      // NSPrintInfo also swaps the paperSize to be 11x8.5,
      // but gui also adds a 90 degree rotation as if it will
      // be drawing on a 8.5x11 page. So, swap 11x8.5 back to
      // 8.5x11 here.
      if ([printInfo orientation] == NSLandscapeOrientation)
	{
	  size = NSMakeSize(size.height, size.width);
	}
    }
  else
    {
      [NSException raise: NSInternalInconsistencyException
		  format: @"current print operation printInfo is nil in %@",
		   NSStringFromSelector(_cmd)];
      return;
    }

  contextType = [context_info objectForKey:
			 NSGraphicsContextRepresentationFormatAttributeName];

  if (contextType)
    {
      if ([contextType isEqual: NSGraphicsContextPSFormat])
        {
          surface = [[CairoPSSurface alloc] initWithDevice: context_info];
          [surface setSize: size];
          // This strange setting is needed because of the way GUI handles offset.
          [CGSTATE GSSetSurface: surface : 0.0 : size.height];
          RELEASE(surface);
        }
      else if ([contextType isEqual: NSGraphicsContextPDFFormat])
        {
          surface = [[CairoPDFSurface alloc] initWithDevice: context_info];
          [surface setSize: size];
          // This strange setting is needed because of the way GUI handles offset.
          [CGSTATE GSSetSurface: surface : 0.0 : size.height];
          RELEASE(surface);
        }
    }
}

- (void) showPage
{
  [CGSTATE showPage];
}

@end

#undef _CAIRO_SURFACE_CLASSNAME
