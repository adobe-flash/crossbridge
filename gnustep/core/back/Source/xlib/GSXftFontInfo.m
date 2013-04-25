/*
   GSXftFontInfo

   NSFont helper for GNUstep GUI X/GPS Backend

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author:  Fred Kiefer <fredkiefer@gmx.de>
   Date: July 2001

   This file is part of the GNUstep GUI X/GPS Backend.

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

#include "config.h"
#include "xlib/XGContext.h"
#include "xlib/XGPrivate.h"
#include "xlib/XGGState.h"
#include "x11/XGServer.h"
#include <Foundation/NSByteOrder.h>
#include <Foundation/NSCharacterSet.h>
#include <Foundation/NSData.h>
#include <Foundation/NSDebug.h>
#include <Foundation/NSDictionary.h>
#include <Foundation/NSValue.h>
// For the encoding functions
#include <GNUstepBase/Unicode.h>

#include <AppKit/NSBezierPath.h>
#include "xlib/GSXftFontInfo.h"

#ifdef HAVE_FC
#define id _gs_avoid_id_collision
#include <fontconfig/fontconfig.h>
#undef id

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H

/*
 * class global dictionary of existing fonts
 */
static NSMutableDictionary *allFonts = nil;

// just a warpper around a FcPattern, to make it a NSObject
@interface FcFont : NSObject
{
  FcPattern *aFont;
}
- initWithPattern:(FcPattern *)aFace;
- (FcPattern *)font;
@end

@implementation FcFont

- initWithPattern:(FcPattern *)aFace
{
  [super init];
  aFont = aFace;
  FcPatternReference(aFace);
  return self;
}

- (FcPattern *)font
{
  return aFont;
}

- (void) dealloc
{
  FcPatternDestroy(aFont);  
  [super dealloc];
}
@end

@implementation FcFontEnumerator

// Make a GNUStep style font descriptor from a FcPattern
static NSArray *faFromFc(FcPattern *pat)
{
  int weight, slant, spacing, nsweight;
  unsigned int nstraits = 0;
  char *family;
  NSMutableString *name, *style;

  if (FcPatternGetInteger(pat, FC_WEIGHT, 0, &weight) != FcResultMatch ||
      FcPatternGetInteger(pat, FC_SLANT,  0, &slant) != FcResultMatch ||
      FcPatternGetString(pat, FC_FAMILY, 0, (FcChar8 **)&family) != FcResultMatch)
    return nil;

  if (FcPatternGetInteger(pat, FC_SPACING, 0, &spacing) == FcResultMatch)
    if (spacing==FC_MONO || spacing==FC_CHARCELL)
      nstraits |= NSFixedPitchFontMask;
  
  name = [NSMutableString stringWithCapacity: 100];
  style = [NSMutableString stringWithCapacity: 100];
  [name appendString: [NSString stringWithUTF8String: family]];

  switch (weight) 
    {
    case FC_WEIGHT_LIGHT:
      [style appendString: @"Light"];
      nsweight = 3;
      break;
    case FC_WEIGHT_MEDIUM:
      nsweight = 6;
      break;
    case FC_WEIGHT_DEMIBOLD:
      [style appendString: @"Demibold"];
      nsweight = 7;
      break;
    case FC_WEIGHT_BOLD:
      [style appendString: @"Bold"];
      nsweight = 9;
      nstraits |= NSBoldFontMask;
      break;
    case FC_WEIGHT_BLACK:
      [style appendString: @"Black"];
      nsweight = 12;
      nstraits |= NSBoldFontMask;
      break;
    default:
      nsweight = 6;
    }

  switch (slant) 
    {
    case FC_SLANT_ROMAN:
      break;
    case FC_SLANT_ITALIC:
      [style appendString: @"Italic"];
      nstraits |= NSItalicFontMask;
      break;
    case FC_SLANT_OBLIQUE:
      [style appendString: @"Oblique"];
      nstraits |= NSItalicFontMask;
      break;
    }

  if ([style length] > 0)
    {
      [name appendString: @"-"];
      [name appendString: style];
    }
  else
    {
      [style appendString: @"Roman"];
    }

  return [NSArray arrayWithObjects: name, 
		  style, 
		  [NSNumber numberWithInt: nsweight], 
		  [NSNumber numberWithUnsignedInt: nstraits],
		  nil];
}

- (void) enumerateFontsAndFamilies
{
  int i;
  NSMutableDictionary *fcxft_allFontFamilies = [[NSMutableDictionary alloc] init];
  NSMutableDictionary *fcxft_allFonts = [[NSMutableDictionary alloc] init];
  NSMutableArray *fcxft_allFontNames = [[NSMutableArray alloc] init];

  FcPattern *pat = FcPatternCreate();
  FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, FC_SLANT, FC_WEIGHT, 
                                     FC_SPACING, NULL);
  FcFontSet *fs = FcFontList(0, pat, os);

  FcPatternDestroy(pat);
  FcObjectSetDestroy(os);

  for (i=0; i < fs->nfont; i++)
    {
      char *family;

      if (FcPatternGetString(fs->fonts[i], FC_FAMILY, 0, (FcChar8 **)&family) == FcResultMatch)
        {
          NSArray *fontArray;

          if ((fontArray = faFromFc(fs->fonts[i])))
            {
	      NSString *familyString;
	      NSMutableArray *familyArray;
              FcFont *aFont;
	      NSString *name = [fontArray objectAtIndex: 0];

              familyString = [NSString stringWithUTF8String: family];
              if (!(familyArray = [fcxft_allFontFamilies objectForKey: familyString]))
                {
                  familyArray = [[NSMutableArray alloc] init];
                  [fcxft_allFontFamilies setObject: familyArray forKey: familyString];
		  RELEASE(familyArray);
                }
              NSDebugLLog(@"NSFont", @"fc enumerator: adding font: %@", name);
              [familyArray addObject: fontArray];
              [fcxft_allFontNames addObject: name];      
              aFont = [[FcFont alloc] initWithPattern: fs->fonts[i]];
              [fcxft_allFonts setObject: aFont forKey: name];
              RELEASE(aFont);
            }
        }
    }
  FcFontSetDestroy (fs); 

  allFontNames = fcxft_allFontNames;
  allFontFamilies = fcxft_allFontFamilies;
  allFonts = fcxft_allFonts;
}

-(NSString *) defaultSystemFontName
{
  if ([allFontNames containsObject: @"Bitstream Vera Sans"])
    return @"Bitstream Vera Sans";
  if ([allFontNames containsObject: @"FreeSans"])
    return @"FreeSans";
  if ([allFontNames containsObject: @"DejaVu Sans"])
    return @"DejaVu Sans";
  return @"Helvetica";
}

-(NSString *) defaultBoldSystemFontName
{
  if ([allFontNames containsObject: @"Bitstream Vera Sans-Bold"])
    return @"Bitstream Vera Sans-Bold";
  if ([allFontNames containsObject: @"FreeSans-Bold"])
    return @"FreeSans-Bold";
  if ([allFontNames containsObject: @"DejaVu Sans-Bold"])
    return @"DejaVu Sans-Bold";
  return @"Helvetica-Bold";
}

-(NSString *) defaultFixedPitchFontName
{
  if ([allFontNames containsObject: @"Bitstream Vera Sans Mono"])
    return @"Bitstream Vera Sans Mono";
  if ([allFontNames containsObject: @"FreeMono"])
    return @"FreeMono";
  if ([allFontNames containsObject: @"DejaVu Sans Mono"])
    return @"DejaVu Sans Mono";
  return @"Courier";
}

@end
#endif

@interface GSXftFontInfo (Private)

- (BOOL) setupAttributes;
- (XGlyphInfo *)xGlyphInfo: (NSGlyph) glyph;

@end

@implementation GSXftFontInfo

- initWithFontName: (NSString*)name
	    matrix: (const CGFloat*)fmatrix
	screenFont: (BOOL)screenFont
{
  if (screenFont)
    {
      RELEASE(self);
      return nil;
    }

  [super init];
  ASSIGN(fontName, name);
  memcpy(matrix, fmatrix, sizeof(matrix));

  if (![self setupAttributes])
    {
      RELEASE(self);
      return nil;
    }

  return self;
}

- (void) dealloc
{
  if (font_info != NULL)
    XftFontClose([XGServer currentXDisplay], (XftFont *)font_info);
  [super dealloc];
}

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#if __INT_MIN__ == 0x7fffffff
#define Ones(mask) __builtin_popcount(mask)
#else
#define Ones(mask) __builtin_popcountl((mask) & 0xffffffff)
#endif
#else
/* Otherwise fall back on HACKMEM 169.  */
static int
Ones(unsigned int n)
{
  register unsigned int tmp;
    
  tmp = n - ((n >> 1) & 033333333333)
      - ((n >> 2) & 011111111111);
  return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}
#endif

- (NSCharacterSet*) coveredCharacterSet
{
  if (coveredCharacterSet == nil)
    {
      if (!((XftFont *)font_info)->charset)
        return nil;
      else
        {
          NSMutableData	*d = [NSMutableData new];
          unsigned count = 0;
          FcCharSet *charset = ((XftFont *)font_info)->charset;
          FcChar32 ucs4;
          FcChar32 map[FC_CHARSET_MAP_SIZE];
          FcChar32 next;
          BOOL swap = NSHostByteOrder() == NS_BigEndian;

          if (!d)
            return nil;

          for (ucs4 = FcCharSetFirstPage(charset, map, &next);
               ucs4 != FC_CHARSET_DONE;
               ucs4 = FcCharSetNextPage(charset, map, &next))
            {
              unsigned int i;
              NSRange aRange;
              unsigned int max;

              aRange = NSMakeRange(ucs4, FC_CHARSET_MAP_SIZE * sizeof(FcChar32));
              max = NSMaxRange(aRange);
              // Round up to a suitable plane size
              max = ((max + 8191) >> 13) << 13;
              [d setLength: max];

              for (i = 0; i < FC_CHARSET_MAP_SIZE; i++)
                if (map[i])
                  {
                    // Do some byte swapping, if needed.
                    if (swap)
                      {
                        map[i] = NSSwapInt(map[i]);
                      }
                    count += Ones(map[i]);
                  }
            
              [d replaceBytesInRange: aRange withBytes: map];
            }

          ASSIGN(coveredCharacterSet, 
                 [NSCharacterSet characterSetWithBitmapRepresentation: d]);
          numberOfGlyphs = count;
          RELEASE(d);
        }
    }

  return coveredCharacterSet;
}

- (CGFloat) widthOfString: (NSString*)string
{
  XGlyphInfo extents;
  int len = [string length];
  XftChar16 str[len]; 

  [string getCharacters: (unichar*)str];
  XftTextExtents16 ([XGServer currentXDisplay],
		    font_info,
		    str, 
		    len,
		    &extents);

  return extents.width;
}

- (CGFloat) widthOfGlyphs: (const NSGlyph *) glyphs length: (int) len
{
  XGlyphInfo extents;
  XftChar16 buf[len];
  int i;

  for (i = 0; i < len; i++)
    {
      buf[i] = glyphs[i];
    }

  XftTextExtents16 ([XGServer currentXDisplay],
		    font_info,
		    buf,
		    len,
		    &extents);

  return extents.width;
}

- (NSMultibyteGlyphPacking)glyphPacking
{
  return NSTwoByteGlyphPacking;
}

- (NSSize) advancementForGlyph: (NSGlyph)glyph
{
  XGlyphInfo *pc = [self xGlyphInfo: glyph];

  // if per_char is NULL assume max bounds
  if (!pc)
    return  NSMakeSize((float)(font_info)->max_advance_width, 0);

  return NSMakeSize((float)pc->xOff, (float)pc->yOff);
}

- (NSRect) boundingRectForGlyph: (NSGlyph)glyph
{
  XGlyphInfo *pc = [self xGlyphInfo: glyph];

  // if per_char is NULL assume max bounds
  if (!pc)
      return NSMakeRect(0.0, 0.0,
		    (float)font_info->max_advance_width,
		    (float)(font_info->ascent + font_info->descent));

  return NSMakeRect((float)pc->x, (float)-pc->y, 
		    (float)(pc->width), 
		    (float)(pc->height));
}

- (BOOL) glyphIsEncoded: (NSGlyph)glyph
{
  return XftGlyphExists([XGServer currentXDisplay],
			(XftFont *)font_info, glyph);
}

- (NSGlyph) glyphWithName: (NSString*)glyphName
{
  // FIXME: There is a mismatch between PS names and X names, that we should 
  // try to correct here
  KeySym k = XStringToKeysym([glyphName cString]);

  if (k == NoSymbol)
    return 0;
  else
    return (NSGlyph)k;
}

- (NSPoint) positionOfGlyph: (NSGlyph)curGlyph
	    precededByGlyph: (NSGlyph)prevGlyph
		  isNominal: (BOOL*)nominal
{
  if (nominal)
    *nominal = YES;

  if (curGlyph == NSControlGlyph || prevGlyph == NSControlGlyph)
    return NSZeroPoint;

//  if (curGlyph == NSNullGlyph)
    {
      NSSize advance = [self advancementForGlyph: prevGlyph];
      return NSMakePoint(advance.width, advance.height);
    }
}

/*
- (CGFloat) pointSize
{
  Display	*xdpy = [XGServer currentXDisplay];

  return XGFontPointSize(xdpy, font_info);
}
*/

- (void) drawString:  (NSString*)string
	  onDisplay: (Display*) xdpy drawable: (Drawable) draw
	       with: (GC) xgcntxt at: (XPoint) xp
{
  NSData *d = [string dataUsingEncoding: mostCompatibleStringEncoding
		      allowLossyConversion: YES];
  int length = [d length];
  const char *cstr = (const char*)[d bytes];
  XGGState *state = (XGGState *)[(XGContext *)GSCurrentContext() currentGState];
  XftDraw *xftdraw = [state xftDrawForDrawable: draw];
  XftColor xftcolor = [state xftColor];

  /* do it */
  XftDrawString16(xftdraw, &xftcolor, font_info, 
		  xp.x, xp.y, (XftChar16*)cstr, length);
}

- (void) drawGlyphs: (const NSGlyph *) glyphs length: (int) len
	  onDisplay: (Display*) xdpy drawable: (Drawable) draw
	       with: (GC) xgcntxt at: (XPoint) xp
{
  XGGState *state = (XGGState *)[(XGContext *)GSCurrentContext() currentGState];
  XftDraw *xftdraw = [state xftDrawForDrawable: draw];
  XftColor xftcolor = [state xftColor];
  XftChar16 buf[len];
  int i;
 
  for (i = 0; i < len; i++)
    {
      buf[i] = glyphs[i];
    }

  /* do it */
  XftDrawString16(xftdraw, &xftcolor, font_info, 
		  xp.x, xp.y, (XftChar16*)buf, len);
}

- (void) draw: (const char*) s length: (int) len 
    onDisplay: (Display*) xdpy drawable: (Drawable) draw
	 with: (GC) xgcntxt at: (XPoint) xp
{
  int length = strlen(s);
  XGGState *state = (XGGState *)[(XGContext *)GSCurrentContext() currentGState];
  XftDraw *xftdraw = [state xftDrawForDrawable: draw];
  XftColor xftcolor = [state xftColor];

#ifdef HAVE_UTF8
  /* do it */
  if (NSUTF8StringEncoding == mostCompatibleStringEncoding)
    {
      XftDrawStringUtf8(xftdraw, &xftcolor, font_info,
                        xp.x, xp.y, (XftChar8 *)s, length);
    }
  else
#endif
    {
      XftDrawString8(xftdraw, &xftcolor, font_info, 
                   xp.x, xp.y, (XftChar8*)s, length);
    }
}

- (CGFloat) widthOf: (const char*) s length: (int) len
{
  XGlyphInfo extents;

#ifdef HAVE_UTF8
  if (mostCompatibleStringEncoding == NSUTF8StringEncoding)
    XftTextExtentsUtf8([XGServer currentXDisplay],
                       font_info,
                       (XftChar8 *)s,
                       len,
                       &extents);
  else
#endif
    XftTextExtents8([XGServer currentXDisplay],
                    font_info,
                    (XftChar8*)s, 
                    len,
                    &extents);

  return extents.width;
}


- (void) setActiveFor: (Display*) xdpy gc: (GC) xgcntxt
{
}

static int bezierpath_move_to(const FT_Vector *to, void *user)
{
  NSBezierPath *path = (NSBezierPath *)user;
  NSPoint d;

  d.x = to->x / 65536.0;
  d.y = to->y / 65536.0;
/*
  d.x = to->x;
  d.y = to->y;
*/
  [path closePath];
  [path moveToPoint: d];
  return 0;
}

static int bezierpath_line_to(const FT_Vector *to, void *user)
{
  NSBezierPath *path = (NSBezierPath *)user;
  NSPoint d;

  d.x = to->x / 65536.0;
  d.y = to->y / 65536.0;
/*
  d.x = to->x;
  d.y = to->y;
*/
  [path lineToPoint: d];
  return 0;
}

static int bezierpath_conic_to(const FT_Vector *c1, const FT_Vector *to, void *user)
{
  NSBezierPath *path = (NSBezierPath *)user;
  NSPoint a, b, c, d;

  a = [path currentPoint];
  d.x = to->x / 65536.0;
  d.y = to->y / 65536.0;
  b.x = c1->x / 65536.0;
  b.y = c1->y / 65536.0;
/*
  d.x = to->x;
  d.y = to->y;
  b.x = c1->x;
  b.y = c1->y;
*/
  c.x = (b.x * 2 + d.x) / 3.0;
  c.y = (b.y * 2 + d.y) / 3.0;
  b.x = (b.x * 2 + a.x) / 3.0;
  b.y = (b.y * 2 + a.y) / 3.0;
  [path curveToPoint: d controlPoint1: b controlPoint2: c];
  return 0;
}

static int bezierpath_cubic_to(const FT_Vector *c1, const FT_Vector *c2, 
                               const FT_Vector *to, void *user)
{
  NSBezierPath *path = (NSBezierPath *)user;
  NSPoint b, c, d;

  b.x = c1->x / 65536.0;
  b.y = c1->y / 65536.0;
  c.x = c2->x / 65536.0;
  c.y = c2->y / 65536.0;
  d.x = to->x / 65536.0;
  d.y = to->y / 65536.0;
/*
  b.x = c1->x;
  b.y = c1->y;
  c.x = c2->x;
  c.y = c2->y;
  d.x = to->x;
  d.y = to->y;
*/
  [path curveToPoint: d controlPoint1: b controlPoint2: c];
  return 0;
}

static FT_Outline_Funcs bezierpath_funcs = {
  move_to: bezierpath_move_to,
  line_to: bezierpath_line_to,
  conic_to: bezierpath_conic_to,
  cubic_to: bezierpath_cubic_to,
  shift: 10,
//  delta: 0,
};

- (void) appendBezierPathWithGlyphs: (NSGlyph *)glyphs
                              count: (int)count
                       toBezierPath: (NSBezierPath *)path
{
  int i;
  FT_Matrix ftmatrix;
  FT_Vector ftdelta;
  FT_Face face;
  FT_Int load_flags = FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP;
  NSPoint p = [path currentPoint];

  ftmatrix.xx = 65536;
  ftmatrix.xy = 0;
  ftmatrix.yx = 0;
  ftmatrix.yy = 65536;
  ftdelta.x = p.x * 64.0;
  ftdelta.y = p.y * 64.0;

  face = XftLockFace((XftFont *)font_info);
  for (i = 0; i < count; i++)
    {
      NSGlyph glyph;
      FT_Glyph gl;
      FT_OutlineGlyph og;

      glyph = glyphs[i];
      // FIXME: Should do this conversion in the glyph creation!
      glyph = XftCharIndex([XGServer currentXDisplay], 
                           (XftFont *)font_info, glyph);
     
      if (FT_Load_Glyph(face, glyph, load_flags))
        continue;

      if (FT_Get_Glyph(face->glyph, &gl))
        continue;

      if (FT_Glyph_Transform(gl, &ftmatrix, &ftdelta))
        {
          NSLog(@"glyph transformation failed!");
          continue;
        }

      og = (FT_OutlineGlyph)gl;

      ftdelta.x += gl->advance.x >> 10;
      ftdelta.y += gl->advance.y >> 10;

      FT_Outline_Decompose(&og->outline, &bezierpath_funcs, path);

      FT_Done_Glyph(gl);
    }

  XftUnlockFace((XftFont *)font_info);

  if (count)
    {
      [path moveToPoint: NSMakePoint(ftdelta.x / 64.0, ftdelta.y / 64.0)];
    }
}

@end

@implementation GSXftFontInfo (Private)

- (BOOL) setupAttributes
{
  Display *xdpy = [XGServer currentXDisplay];
  int defaultScreen = DefaultScreen(xdpy);
  NSString *weightString = nil;

#ifdef HAVE_FC
  FcFont *realFont = [allFonts objectForKey: fontName];
  FcPattern *fontPattern;
  FcPattern *pattern; 
  FcResult fc_result;
  char *family;
  int fcspacing, fcweight, fcslant;

  if (!realFont)
    {
      return NO;
    }

  if (!xdpy)
    return NO;

  fontPattern = FcPatternDuplicate([realFont font]);

  // the only thing needs customization here is the size
  // FIXME: It would be correcter to use FC_SIZE as GNUstep should be
  // using point measurements, but as the rest of the library uses pixel,
  // we need to stick with that here.
  FcPatternAddDouble(fontPattern, FC_PIXEL_SIZE, (double)(matrix[0]));
  // Should do this only when size > 8
  FcPatternAddBool(fontPattern, FC_AUTOHINT, FcTrue);
  pattern = XftFontMatch(xdpy, defaultScreen, fontPattern, &fc_result);
  // tide up
  FcPatternDestroy(fontPattern);
  
  if (FcPatternGetString(pattern, FC_FAMILY, 0, (FcChar8 **)&family) == FcResultMatch)
    {
      ASSIGN(familyName, [NSString stringWithUTF8String: (const char*)family]);
    }
  if (FcPatternGetInteger(pattern, FC_SPACING, 0, &fcspacing) == FcResultMatch)
    {
      isFixedPitch = (fcspacing == FC_MONO || fcspacing == FC_CHARCELL);
    }
  if (FcPatternGetInteger(pattern, FC_WEIGHT, 0, &fcweight) == FcResultMatch)
    {
      switch (fcweight)
        {
        case FC_WEIGHT_LIGHT:
            weight = 3;
            weightString = @"light";
            break;
        case FC_WEIGHT_MEDIUM:
            weight = 6;
            weightString = @"medium";
            break;
        case FC_WEIGHT_DEMIBOLD:
            weight = 7;
            weightString = @"demibold";
            break;
        case FC_WEIGHT_BOLD:
            weight = 9;
            weightString = @"bold";
            break;
        case FC_WEIGHT_BLACK:
            weight = 12;
            weightString = @"black";
            break;
	default:
          // Don't know
          weight = 6;
          weightString = @"medium";
	}
    }

  if (FcPatternGetInteger(pattern, FC_SLANT,  0, &fcslant) == FcResultMatch)
    {
      switch (fcslant) 
        {
        case FC_SLANT_ROMAN:
          traits |= NSUnitalicFontMask;
          break;
        case FC_SLANT_ITALIC:
          traits |= NSItalicFontMask;
          break;
        case FC_SLANT_OBLIQUE:
          traits |= NSItalicFontMask;
          break;
       }
    }

  // Derek Zhou claims that this takes over the ownership of the pattern
  if ((font_info = XftFontOpenPattern(xdpy, pattern)))
    {
      NSDebugLLog(@"NSFont", @"Loaded font: %@", fontName);
    }
  else
    {
      NSDebugLLog(@"NSFont", @"Cannot load font: %@", fontName);
      return NO;
    }

  /* TODO: somehow make gnustep-gui send unicode our way. utf8? ugly, but it works */
  mostCompatibleStringEncoding = NSUTF8StringEncoding;
  encodingScheme = @"iso10646-1";
#else 
  NSString *reg;
  XftPattern *pattern;
  XftResult result;
  NSString *xfontname;
  char *xftTypeString;
  int xftTypeInt;
  NSArray *encoding;

  if (!xdpy)
    return NO;

  // Retrieve the XLFD matching the given fontName. DPS->X.
  xfontname = XGXFontName(fontName, matrix[0]);

  // Load Xft font and get font info structure.
  if ((xfontname == nil) ||
      (font_info = XftFontOpenXlfd(xdpy, defaultScreen, [xfontname UTF8String])) == NULL)
    {
      NSLog(@"Unable to open fixed font %@", xfontname);
      return NO;
    }
  else
    NSDebugLog(@"Loaded font: %@", xfontname);

  pattern = font_info->pattern;
  result = XftPatternGetString(pattern, XFT_FAMILY, 0, &xftTypeString);
  if (result != XftResultTypeMismatch)
    {
      ASSIGN(familyName,
         [NSString stringWithUTF8String: (const char*)xftTypeString]);
    }
  result = XftPatternGetInteger(pattern, XFT_SPACING, 0, &xftTypeInt);
  if (result != XftResultTypeMismatch)
    {
      isFixedPitch = (xftTypeInt != 0);
    }

  result = XftPatternGetInteger(pattern, XFT_WEIGHT, 0, &xftTypeInt);
  if (result != XftResultTypeMismatch)
    {
      switch (xftTypeInt)
        {
        case 0:
          weight = 3;
          weightString = @"light";
          break;
        case 100:
          weight = 6;
          weightString = @"medium";
          break;
        case 180:
          weight = 7;
          weightString = @"demibold";
          break;
        case 200:
          weight = 9;
          weightString = @"bold";
          break;
        case 210:
          weight = 12;
          weightString = @"black";
          break;
        default:
          // Don't know
          weight = 6;;
        }
    }

  result = XftPatternGetInteger(pattern, XFT_SLANT, 0, &xftTypeInt);
  if (result != XftResultTypeMismatch)
    {
      if (xftTypeInt != 0)
        traits |= NSItalicFontMask;
      else
        traits |= NSUnitalicFontMask;
    }

  XftPatternGetString(pattern, XFT_ENCODING, 0, &xftTypeString);
  encodingScheme = [NSString stringWithUTF8String: xftTypeString];
  encoding = [encodingScheme componentsSeparatedByString: @"-"];
  reg = [encoding objectAtIndex: 0];
  if (reg != nil)
    { 
      if ([encoding count] > 1)
        {
	  NSString *enc = [encoding lastObject];
	  mostCompatibleStringEncoding = GSEncodingForRegistry(reg, enc);
	  if (mostCompatibleStringEncoding == GSUndefinedEncoding)
	    mostCompatibleStringEncoding = NSASCIIStringEncoding;
          if (mostCompatibleStringEncoding == NSUnicodeStringEncoding)
            mostCompatibleStringEncoding = NSUTF8StringEncoding;
	  RETAIN(encodingScheme);
	}
    }
  else
    encodingScheme = nil;
  // FIXME: italicAngle, underlinePosition, underlineThickness are not set.
  // Should use XA_ITALIC_ANGLE, XA_UNDERLINE_POSITION, XA_UNDERLINE_THICKNESS
#endif

  // Fill the ivars
  if (weight >= 9)
    traits |= NSBoldFontMask;
  else
    traits |= NSUnboldFontMask;

  if (isFixedPitch)
    traits |= NSFixedPitchFontMask;

  isBaseFont = NO;
  ascender = font_info->ascent;
  descender = -(font_info->descent);
  capHeight = ascender - descender;   // TODO
  xHeight = capHeight*0.6;   //Errr... TODO
  fontBBox = NSMakeRect(
    (float)(0),
    (float)(-font_info->descent),
    (float)(font_info->max_advance_width),
    (float)(font_info->ascent + font_info->descent));
  maximumAdvancement = NSMakeSize(font_info->max_advance_width, 0.0);
  minimumAdvancement = NSMakeSize(0,0);
//   printf("h=%g  a=%g d=%g  max=(%g %g)  (%g %g)+(%g %g)\n",
//          xHeight, ascender, descender,
//          maximumAdvancement.width, maximumAdvancement.height,
//          fontBBox.origin.x, fontBBox.origin.y,
//          fontBBox.size.width, fontBBox.size.height);

  return YES;
}

- (XGlyphInfo *)xGlyphInfo: (NSGlyph) glyph
{
  static XGlyphInfo glyphInfo;

  XftTextExtents32 ([XGServer currentXDisplay],
                    (XftFont *)font_info,
                    &glyph,
                    1,
                    &glyphInfo);

  return &glyphInfo;
}

@end
