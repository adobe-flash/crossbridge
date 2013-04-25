/** <title>NSGraphicsContext</title>

   <abstract>Abstract drawing context class.</abstract>

   Copyright (C) 1998,1999 Free Software Foundation, Inc.

   Author: Richard Frith-Macdonald <richard@brainstorm.co.uk>
   Date: Feb 1999
   Based on code by:  Adam Fedor <fedor@gnu.org>
   Date: Nov 1998
   
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

#ifndef _NSGraphicsContext_h_INCLUDE
#define _NSGraphicsContext_h_INCLUDE
#import <GNUstepBase/GSVersionMacros.h>

#import <Foundation/NSObject.h>
#import <Foundation/NSMapTable.h>

#import <AppKit/AppKitDefines.h>

@class NSDate;
@class NSDictionary;
@class NSMutableArray;
@class NSMutableData;
@class NSMutableSet;
@class NSString;
@class NSView;
@class NSWindow;
@class NSFont;
@class NSSet;
@class NSBitmapImageRep;
@class NSGradient;

/*
 * Backing Store Types
 */
typedef enum _NSBackingStoreType
{
  NSBackingStoreRetained,
  NSBackingStoreNonretained,
  NSBackingStoreBuffered

} NSBackingStoreType;

/** NSCompositingOperation defines how an image is drawn or composited onto another.
 * <list>
 *  <item>NSCompositeClear: Cleans out an area</item>
 *  <item> NSCompositeCopy: <em>(Common)</em> Draws over the destination replacing it and carrying over transparency to the destination</item>
 *  <item> NSCompositeSourceOver: <em>(Common)</em> Draws over the destination with transparency</item>
 *  <item> NSCompositeSourceIn</item>
 *  <item> NSCompositeSourceOut</item>
 *  <item> NSCompositeSourceAtop</item>
 *  <item> NSCompositeDestinationOver</item>
 *  <item> NSCompositeDestinationIn</item>
 *  <item> NSCompositeDestinationOut</item>
 *  <item> NSCompositeDestinationAtop</item>
 *  <item> NSCompositeXOR</item>
 *  <item> NSCompositePlusDarker</item>
 *  <item> NSCompositeHighlight</item>
 *  <item> NSCompositePlusLighter</item>
 * </list>
 */
typedef enum _NSCompositingOperation
{
 
  NSCompositeClear,
  NSCompositeCopy,
  NSCompositeSourceOver,
  NSCompositeSourceIn,
  NSCompositeSourceOut,
  NSCompositeSourceAtop,
  NSCompositeDestinationOver,
  NSCompositeDestinationIn,
  NSCompositeDestinationOut,
  NSCompositeDestinationAtop,
  NSCompositeXOR,
  NSCompositePlusDarker,
  NSCompositeHighlight,
  NSCompositePlusLighter

#if OS_API_VERSION(GS_API_NONE, GS_API_NONE)
  , GSCompositeHighlight = 100
#endif
} NSCompositingOperation;

typedef int NSWindowDepth;

/* Image interpolation */
typedef enum _NSImageInterpolation
{
  NSImageInterpolationDefault,
  NSImageInterpolationNone,
  NSImageInterpolationLow,
  NSImageInterpolationHigh
} NSImageInterpolation;


/*
 * The following graphics context stuff is needed by inline functions,
 * so it must always be available even when compiling for MACOSX or OPENSTEP
 */


typedef enum _GSTextDrawingMode
{
  GSTextFill,
  GSTextStroke,
  GSTextClip
} GSTextDrawingMode;

// We have to load this after the NSCompositingOperation are defined!!!
#import <GNUstepGUI/GSMethodTable.h>

/*
 * Window ordering
 */
typedef enum _NSWindowOrderingMode
{
  NSWindowAbove,
  NSWindowBelow,
  NSWindowOut

} NSWindowOrderingMode;

/*
 * Window input state
 */
typedef enum _GSWindowInputState
{
  GSTitleBarKey = 0,
  GSTitleBarNormal = 1,
  GSTitleBarMain = 2

} GSWindowInputState;

/* Color spaces */
typedef enum _GSColorSpace
{
  GSDeviceGray,
  GSDeviceRGB,
  GSDeviceCMYK,
  GSCalibratedGray,
  GSCalibratedRGB,
  GSCIELab,
  GSICC
} GSColorSpace;

@interface NSGraphicsContext : NSObject
{
  /* Make the one public instance variable first in the object so that, if we
   * add or remove others, we don't necessarily need to recompile everything.
   */
@public
  const gsMethodTable	*methods;

@protected
  NSDictionary		*context_info;
  NSMutableData		*context_data;
  NSMutableArray	*focus_stack;
  NSMutableSet          *usedFonts;
  NSImageInterpolation  _interp;
  BOOL                  _antialias;
  NSPoint _patternPhase;
  void *_graphicsPort;
  BOOL _isFlipped;
  NSCompositingOperation _compositingOperation;
}

+ (BOOL) currentContextDrawingToScreen;
+ (NSGraphicsContext *) graphicsContextWithAttributes: (NSDictionary *)attributes;
+ (NSGraphicsContext *) graphicsContextWithWindow: (NSWindow *)aWindow;

+ (void) restoreGraphicsState;
+ (void) saveGraphicsState;
+ (void) setGraphicsState: (int)graphicsState;
+ (void) setCurrentContext: (NSGraphicsContext*)context;
+ (NSGraphicsContext*) currentContext;
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
+ (NSGraphicsContext *) graphicsContextWithBitmapImageRep: (NSBitmapImageRep *)bitmap;
+ (NSGraphicsContext *) graphicsContextWithGraphicsPort: (void *)port 
                                                flipped: (BOOL)flag;
#endif

- (NSDictionary *) attributes;
- (void *) graphicsPort;

- (BOOL) isDrawingToScreen;
- (void) flushGraphics;
- (void) restoreGraphicsState;
- (void) saveGraphicsState;

- (void *) focusStack;
- (void) setFocusStack: (void *)stack;

- (void) setImageInterpolation: (NSImageInterpolation)interpolation;
- (NSImageInterpolation) imageInterpolation;
- (void) setShouldAntialias: (BOOL)antialias;
- (BOOL) shouldAntialias;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_2, GS_API_LATEST)
- (NSPoint) patternPhase;
- (void) setPatternPhase: (NSPoint)phase;
#endif

#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
- (NSCompositingOperation) compositingOperation;
- (void) setCompositingOperation:(NSCompositingOperation) operation;

- (BOOL) isFlipped;
#endif

@end

APPKIT_EXPORT NSGraphicsContext	*GSCurrentContext(void);

#if OS_API_VERSION(GS_API_NONE, GS_API_NONE)

@interface NSGraphicsContext (GNUstep)
+ (void) setDefaultContextClass: (Class)defaultContextClass;

- (id) initWithContextInfo: (NSDictionary*)info;

/*
 * Focus management methods - lock and unlock should only be used by NSView
 * in it's implementation of lockFocus and unlockFocus.
 */
- (NSView*) focusView;
- (void) lockFocusView: (NSView*)aView inRect: (NSRect)rect;
- (void) unlockFocusView: (NSView*)aView needsFlush: (BOOL)flush;

/* Private methods for printing */
- (void) useFont: (NSString *)fontName;
- (void) resetUsedFonts;
- (NSSet *) usedFonts;

/* Private backend methods */
+ (void) handleExposeRect: (NSRect)rect forDriver: (void *)driver;
@end
#endif


/*
 *	GNUstep drawing engine extensions - these are the methods actually
 *	called when one of the inline PostScript functions (like PSlineto())
 *	is called.
 */
@interface NSGraphicsContext (Ops)
/* ----------------------------------------------------------------------- */
/* Color operations */
/* ----------------------------------------------------------------------- */
- (void) DPScurrentalpha: (float*)a;
- (void) DPScurrentcmykcolor: (float*)c : (float*)m : (float*)y : (float*)k;
- (void) DPScurrentgray: (float*)gray;
- (void) DPScurrenthsbcolor: (float*)h : (float*)s : (float*)b;
- (void) DPScurrentrgbcolor: (float*)r : (float*)g : (float*)b;
- (void) DPSsetalpha: (float)a;
- (void) DPSsetcmykcolor: (float)c : (float)m : (float)y : (float)k;
- (void) DPSsetgray: (float)gray;
- (void) DPSsethsbcolor: (float)h : (float)s : (float)b;
- (void) DPSsetrgbcolor: (float)r : (float)g : (float)b;

- (void) GSSetPatterColor: (NSImage*)image;

- (void) GSSetFillColorspace: (void *)spaceref;
- (void) GSSetStrokeColorspace: (void *)spaceref;
- (void) GSSetFillColor: (const CGFloat *)values;
- (void) GSSetStrokeColor: (const CGFloat *)values;

/* ----------------------------------------------------------------------- */
/* Text operations */
/* ----------------------------------------------------------------------- */
- (void) DPSashow: (float)x : (float)y : (const char*)s;
- (void) DPSawidthshow: (float)cx : (float)cy : (int)c : (float)ax : (float)ay 
		      : (const char*)s;
- (void) DPScharpath: (const char*)s : (int)b;
- (void) appendBezierPathWithPackedGlyphs: (const char *)packedGlyphs
                                     path: (NSBezierPath*)aPath;
- (void) DPSshow: (const char*)s;
- (void) DPSwidthshow: (float)x : (float)y : (int)c : (const char*)s;
- (void) DPSxshow: (const char*)s : (const float*)numarray : (int)size;
- (void) DPSxyshow: (const char*)s : (const float*)numarray : (int)size;
- (void) DPSyshow: (const char*)s : (const float*)numarray : (int)size;

- (void) GSSetCharacterSpacing: (float)extra;
- (void) GSSetFont: (void *)fontref;
- (void) GSSetFontSize: (float)size;
- (NSAffineTransform *) GSGetTextCTM;
- (NSPoint) GSGetTextPosition;
- (void) GSSetTextCTM: (NSAffineTransform *)ctm;
- (void) GSSetTextDrawingMode: (GSTextDrawingMode)mode;
- (void) GSSetTextPosition: (NSPoint)loc;
- (void) GSShowText: (const char *)string : (size_t) length;
- (void) GSShowGlyphs: (const NSGlyph *)glyphs : (size_t) length;
- (void) GSShowGlyphsWithAdvances: (const NSGlyph *)glyphs : (const NSSize *)advances : (size_t) length;

/* ----------------------------------------------------------------------- */
/* Gstate Handling */
/* ----------------------------------------------------------------------- */
- (void) DPSgrestore;
- (void) DPSgsave;
- (void) DPSinitgraphics;
- (void) DPSsetgstate: (int)gst;

- (int)  GSDefineGState;
- (void) GSUndefineGState: (int)gst;
- (void) GSReplaceGState: (int)gst;

/* ----------------------------------------------------------------------- */
/* Gstate operations */
/* ----------------------------------------------------------------------- */
- (void) DPScurrentflat: (float*)flatness;
- (void) DPScurrentlinecap: (int*)linecap;
- (void) DPScurrentlinejoin: (int*)linejoin;
- (void) DPScurrentlinewidth: (float*)width;
- (void) DPScurrentmiterlimit: (float*)limit;
- (void) DPScurrentpoint: (float*)x : (float*)y;
- (void) DPScurrentstrokeadjust: (int*)b;
- (void) DPSsetdash: (const float*)pat : (int)size : (float)offset;
- (void) DPSsetflat: (float)flatness;
- (void) DPSsethalftonephase: (float)x : (float)y;
- (void) DPSsetlinecap: (int)linecap;
- (void) DPSsetlinejoin: (int)linejoin;
- (void) DPSsetlinewidth: (float)width;
- (void) DPSsetmiterlimit: (float)limit;
- (void) DPSsetstrokeadjust: (int)b;

/* ----------------------------------------------------------------------- */
/* Matrix operations */
/* ----------------------------------------------------------------------- */
- (void) DPSconcat: (const float*)m;
- (void) DPSinitmatrix;
- (void) DPSrotate: (float)angle;
- (void) DPSscale: (float)x : (float)y;
- (void) DPStranslate: (float)x : (float)y;

- (NSAffineTransform *) GSCurrentCTM;
- (void) GSSetCTM: (NSAffineTransform *)ctm;
- (void) GSConcatCTM: (NSAffineTransform *)ctm;

/* ----------------------------------------------------------------------- */
/* Paint operations */
/* ----------------------------------------------------------------------- */
- (void) DPSarc: (float)x : (float)y : (float)r : (float)angle1 
	       : (float)angle2;
- (void) DPSarcn: (float)x : (float)y : (float)r : (float)angle1 
		: (float)angle2;
- (void) DPSarct: (float)x1 : (float)y1 : (float)x2 : (float)y2 : (float)r;
- (void) DPSclip;
- (void) DPSclosepath;
- (void) DPScurveto: (float)x1 : (float)y1 : (float)x2 : (float)y2 
		   : (float)x3 : (float)y3;
- (void) DPSeoclip;
- (void) DPSeofill;
- (void) DPSfill;
- (void) DPSflattenpath;
- (void) DPSinitclip;
- (void) DPSlineto: (float)x : (float)y;
- (void) DPSmoveto: (float)x : (float)y;
- (void) DPSnewpath;
- (void) DPSpathbbox: (float*)llx : (float*)lly : (float*)urx : (float*)ury;
- (void) DPSrcurveto: (float)x1 : (float)y1 : (float)x2 : (float)y2 
		    : (float)x3 : (float)y3;
- (void) DPSrectclip: (float)x : (float)y : (float)w : (float)h;
- (void) DPSrectfill: (float)x : (float)y : (float)w : (float)h;
- (void) DPSrectstroke: (float)x : (float)y : (float)w : (float)h;
- (void) DPSreversepath;
- (void) DPSrlineto: (float)x : (float)y;
- (void) DPSrmoveto: (float)x : (float)y;
- (void) DPSstroke;
- (void) DPSshfill: (NSDictionary *)shaderDictionary;

- (void) GSSendBezierPath: (NSBezierPath *)path;
- (void) GSRectClipList: (const NSRect *)rects : (int) count;
- (void) GSRectFillList: (const NSRect *)rects : (int) count;

/* ----------------------------------------------------------------------- */
/* Window system ops */
/* ----------------------------------------------------------------------- */
- (void) GSCurrentDevice: (void**)device : (int*)x : (int*)y;
- (void) GSSetDevice: (void*)device : (int)x : (int)y;
- (void) DPScurrentoffset: (int*)x : (int*)y;
- (void) DPSsetoffset: (short int)x : (short int)y;

/*-------------------------------------------------------------------------*/
/* Graphics Extensions Ops */
/*-------------------------------------------------------------------------*/
- (void) DPScomposite: (float)x : (float)y : (float)w : (float)h 
		     : (int)gstateNum : (float)dx : (float)dy : (int)op;
- (void) DPScompositerect: (float)x : (float)y : (float)w : (float)h : (int)op;
- (void) DPSdissolve: (float)x : (float)y : (float)w : (float)h 
		    : (int)gstateNum : (float)dx : (float)dy : (float)delta;

- (void) GScomposite: (int)gstateNum 
             toPoint: (NSPoint)aPoint
	    fromRect: (NSRect)srcRect
	   operation: (NSCompositingOperation)op
	    fraction: (float)delta;
- (BOOL) supportsDrawGState;
- (void) GSdraw: (int)gstateNum
        toPoint: (NSPoint)aPoint
       fromRect: (NSRect)srcRect
      operation: (NSCompositingOperation)op
       fraction: (float)delta;
- (void) GSDrawImage: (NSRect)rect : (void *)imageref;

/* ----------------------------------------------------------------------- */
/* Postscript Client functions */
/* ----------------------------------------------------------------------- */
- (void) DPSPrintf: (const char *)fmt : (va_list)args;
- (void) DPSWriteData: (const char *)buf : (unsigned int)count;

@end

/* ----------------------------------------------------------------------- */
/* NSGraphics Ops */	
/* ----------------------------------------------------------------------- */
@interface NSGraphicsContext (NSGraphics)

/**
<p>
Read raw pixels from the device and return the information as a bitmap.
Pixels are read from the smallest device-pixel aligned rectangle
containing rect (defined in the current graphics state and clipped to
the current window, but not against the clipping path). If the resulting
device rectangle is degenerate, Size will be (0,0) and Data will be nil,
but the other entries in the dictionary will be filled in.
</p><p>
If the device does not support the operation, returns nil.
</p><p>
The returned dictionary contains at least the following keys:
</p>
<deflist>
<term>Data</term><desc>An NSData-instance with the image data.</desc>

<term>Size</term><desc>An NSValue/NSSize with the size in pixels of the
returned image data.</desc>

<term>BitsPerSample</term><desc>An NSValue/unsigned int.</desc>

<term>SamplesPerPixel</term><desc>An NSValue/unsigned int.</desc>

<term>ColorSpace</term><desc>An NSString with the name of the color space the
data is in.</desc>

<term>HasAlpha</term><desc>An NSValue/unsigned int. 0 if the returned image
does not have an alpha channel, 1 if it does.</desc>

<term>Matrix</term><desc>An NSAffineTransform-instance that contains the
transform between current user space and image space for this image.</desc>
</deflist>
*/
- (NSDictionary *) GSReadRect: (NSRect)rect;

/* Soon to be obsolete */
- (void) NSDrawBitmap: (NSRect) rect : (int) pixelsWide : (int) pixelsHigh
		     : (int) bitsPerSample : (int) samplesPerPixel 
		     : (int) bitsPerPixel : (int) bytesPerRow : (BOOL) isPlanar
		     : (BOOL) hasAlpha : (NSString *) colorSpaceName
		     : (const unsigned char *const [5]) data;

- (void) NSBeep;

/* Context helper wraps */
- (void) GSWSetViewIsFlipped: (BOOL) flipped;
- (BOOL) GSWViewIsFlipped;

@end

/* ----------------------------------------------------------------------- */
/* Printing Ops */	
/* ----------------------------------------------------------------------- */
@interface NSGraphicsContext (Printing)

- (void) beginPage: (int)ordinalNum
             label: (NSString*)aString
              bBox: (NSRect)pageRect
             fonts: (NSString*)fontNames;
- (void) beginPrologueBBox: (NSRect)boundingBox
              creationDate: (NSString*)dateCreated
                 createdBy: (NSString*)anApplication
                     fonts: (NSString*)fontNames
                   forWhom: (NSString*)user
                     pages: (int)numPages
                     title: (NSString*)aTitle;
- (void) beginSetup;
- (void) beginTrailer;
- (void) endDocumentPages: (int)pages
            documentFonts: (NSSet*)fontNames;
- (void) endHeaderComments;
- (void) endPageSetup;
- (void) endPrologue;
- (void) endSetup;
- (void) endSheet;
- (void) endTrailer;
- (void) printerProlog;
- (void) showPage;

@end

@interface NSGraphicsContext (NSGradient)
- (void) drawGradient: (NSGradient*)gradient
           fromCenter: (NSPoint)startCenter
               radius: (CGFloat)startRadius
             toCenter: (NSPoint)endCenter 
               radius: (CGFloat)endRadius
              options: (NSUInteger)options;

- (void) drawGradient: (NSGradient*)gradient
            fromPoint: (NSPoint)startPoint
              toPoint: (NSPoint)endPoint
              options: (NSUInteger)options;

@end

/* NSGraphicContext constants */
APPKIT_EXPORT NSString *NSGraphicsContextDestinationAttributeName;
APPKIT_EXPORT NSString *NSGraphicsContextPDFFormat;
APPKIT_EXPORT NSString *NSGraphicsContextPSFormat;
APPKIT_EXPORT NSString *NSGraphicsContextRepresentationFormatAttributeName;

/* Colorspace constants */
APPKIT_EXPORT NSString *GSColorSpaceName;
APPKIT_EXPORT NSString *GSColorSpaceWhitePoint;
APPKIT_EXPORT NSString *GSColorSpaceBlackPoint;
APPKIT_EXPORT NSString *GSColorSpaceGamma;
APPKIT_EXPORT NSString *GSColorSpaceMatrix;
APPKIT_EXPORT NSString *GSColorSpaceRange;
APPKIT_EXPORT NSString *GSColorSpaceComponents;
APPKIT_EXPORT NSString *GSColorSpaceProfile;
APPKIT_EXPORT NSString *GSAlternateColorSpace;
APPKIT_EXPORT NSString *GSBaseColorSpace;
APPKIT_EXPORT NSString *GSColorSpaceColorTable;

#endif /* _NSGraphicsContext_h_INCLUDE */

