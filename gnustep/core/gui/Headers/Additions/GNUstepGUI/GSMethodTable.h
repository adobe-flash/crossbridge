/* GSMethodTable.h - Definitions of PostScript methods for NSGraphicsContext

   Copyright (C) 1998 Free Software Foundation, Inc.
   Written by:  Adam Fedor <fedor@gnu.org>
   Date: Nov 1998
   Updated by:  Richard Frith-Macdonald <richard@brainstorm.co.uk>
   
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

#ifndef _GSMethodTable_h_INCLUDE
#define _GSMethodTable_h_INCLUDE

#import <Foundation/NSObject.h>
#import <Foundation/NSGeometry.h>
#import <AppKit/NSFont.h>

@class NSAffineTransform;
@class NSDate;
@class NSString;
@class NSBezierPath;
@class NSColor;
@class NSEvent;
@class NSGraphicsContext;

typedef struct {

/* ----------------------------------------------------------------------- */
/* Color operations */
/* ----------------------------------------------------------------------- */
  void (*DPScurrentalpha_)
        (NSGraphicsContext*, SEL, float*);
  void (*DPScurrentcmykcolor____)
        (NSGraphicsContext*, SEL, float*, float*, float*, float*);
  void (*DPScurrentgray_)
        (NSGraphicsContext*, SEL, float*);
  void (*DPScurrenthsbcolor___)
        (NSGraphicsContext*, SEL, float*, float*, float*);
  void (*DPScurrentrgbcolor___)
        (NSGraphicsContext*, SEL, float*, float*, float*);
  void (*DPSsetalpha_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSsetcmykcolor____)
        (NSGraphicsContext*, SEL, float, float, float, float);
  void (*DPSsetgray_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSsethsbcolor___)
        (NSGraphicsContext*, SEL, float, float, float);
  void (*DPSsetrgbcolor___)
        (NSGraphicsContext*, SEL, float, float, float);

  void (*GSSetFillColorspace_)
        (NSGraphicsContext*, SEL, NSDictionary *);
  void (*GSSetStrokeColorspace_)
        (NSGraphicsContext*, SEL, NSDictionary *);
  void (*GSSetFillColor_)
        (NSGraphicsContext*, SEL, float *);
  void (*GSSetStrokeColor_)
        (NSGraphicsContext*, SEL, float *);

/* ----------------------------------------------------------------------- */
/* Text operations */
/* ----------------------------------------------------------------------- */
  void (*DPSashow___)
        (NSGraphicsContext*, SEL, float, float, const char*);
  void (*DPSawidthshow______)
        (NSGraphicsContext*, SEL, float, float, int, float, float, const char*);
  void (*DPScharpath__)
        (NSGraphicsContext*, SEL, const char*, int);
  void (*DPSshow_)
        (NSGraphicsContext*, SEL, const char*);
  void (*DPSwidthshow____)
        (NSGraphicsContext*, SEL, float, float, int, const char*);
  void (*DPSxshow___)
        (NSGraphicsContext*, SEL, const char*, const float*, int);
  void (*DPSxyshow___)
        (NSGraphicsContext*, SEL, const char*, const float*, int);
  void (*DPSyshow___)
        (NSGraphicsContext*, SEL, const char*, const float*, int);

  void (*GSSetCharacterSpacing_)
        (NSGraphicsContext*, SEL, float);
  void (*GSSetFont_)
        (NSGraphicsContext*, SEL, NSFont*);
  void (*GSSetFontSize_)
        (NSGraphicsContext*, SEL, float);
  NSAffineTransform * (*GSGetTextCTM)
        (NSGraphicsContext*, SEL);
  NSPoint (*GSGetTextPosition)
        (NSGraphicsContext*, SEL);
  void (*GSSetTextCTM_)
        (NSGraphicsContext*, SEL, NSAffineTransform *);
  void (*GSSetTextDrawingMode_)
        (NSGraphicsContext*, SEL, GSTextDrawingMode);
  void (*GSSetTextPosition_)
        (NSGraphicsContext*, SEL, NSPoint);
  void (*GSShowText__)
        (NSGraphicsContext*, SEL, const char *, size_t);
  void (*GSShowGlyphs__)
        (NSGraphicsContext*, SEL, const NSGlyph *, size_t);
  void (*GSShowGlyphsWithAdvances__)
        (NSGraphicsContext*, SEL, const NSGlyph *, const NSSize *, size_t);

/* ----------------------------------------------------------------------- */
/* Gstate Handling */
/* ----------------------------------------------------------------------- */
  void (*DPSgrestore)
        (NSGraphicsContext*, SEL);
  void (*DPSgsave)
        (NSGraphicsContext*, SEL);
  void (*DPSinitgraphics)
        (NSGraphicsContext*, SEL);
  void (*DPSsetgstate_)
        (NSGraphicsContext*, SEL, int);

  int (*GSDefineGState)
        (NSGraphicsContext*, SEL);
  void (*GSUndefineGState_)
        (NSGraphicsContext*, SEL, int);
  void (*GSReplaceGState_)
        (NSGraphicsContext*, SEL, int);

/* ----------------------------------------------------------------------- */
/* Gstate operations */
/* ----------------------------------------------------------------------- */
  void (*DPScurrentflat_)
        (NSGraphicsContext*, SEL, float*);
  void (*DPScurrentlinecap_)
        (NSGraphicsContext*, SEL, int*);
  void (*DPScurrentlinejoin_)
        (NSGraphicsContext*, SEL, int*);
  void (*DPScurrentlinewidth_)
        (NSGraphicsContext*, SEL, float*);
  void (*DPScurrentmiterlimit_)
        (NSGraphicsContext*, SEL, float*);
  void (*DPScurrentpoint__)
        (NSGraphicsContext*, SEL, float*, float*);
  void (*DPScurrentstrokeadjust_)
        (NSGraphicsContext*, SEL, int*);
  void (*DPSsetdash___)
        (NSGraphicsContext*, SEL, const float*, int, float);
  void (*DPSsetflat_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSsethalftonephase__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPSsetlinecap_)
        (NSGraphicsContext*, SEL, int);
  void (*DPSsetlinejoin_)
        (NSGraphicsContext*, SEL, int);
  void (*DPSsetlinewidth_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSsetmiterlimit_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSsetstrokeadjust_)
        (NSGraphicsContext*, SEL, int);

/* ----------------------------------------------------------------------- */
/* Matrix operations */
/* ----------------------------------------------------------------------- */
  void (*DPSconcat_)
        (NSGraphicsContext*, SEL, const float*);
  void (*DPSinitmatrix)
        (NSGraphicsContext*, SEL);
  void (*DPSrotate_)
        (NSGraphicsContext*, SEL, float);
  void (*DPSscale__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPStranslate__)
        (NSGraphicsContext*, SEL, float, float);

  NSAffineTransform * (*GSCurrentCTM)
        (NSGraphicsContext*, SEL);
  void (*GSSetCTM_)
        (NSGraphicsContext*, SEL, NSAffineTransform *);
  void (*GSConcatCTM_)
        (NSGraphicsContext*, SEL, NSAffineTransform *);

/* ----------------------------------------------------------------------- */
/* Paint operations */
/* ----------------------------------------------------------------------- */
  void (*DPSarc_____)
        (NSGraphicsContext*, SEL, float, float, float, float, float);
  void (*DPSarcn_____)
        (NSGraphicsContext*, SEL, float, float, float, float, float);
  void (*DPSarct_____)
        (NSGraphicsContext*, SEL, float, float, float, float, float);
  void (*DPSclip)
        (NSGraphicsContext*, SEL);
  void (*DPSclosepath)
        (NSGraphicsContext*, SEL);
  void (*DPScurveto______)
        (NSGraphicsContext*, SEL, float, float, float, float, float, float);
  void (*DPSeoclip)
        (NSGraphicsContext*, SEL);
  void (*DPSeofill)
        (NSGraphicsContext*, SEL);
  void (*DPSfill)
        (NSGraphicsContext*, SEL);
  void (*DPSflattenpath)
        (NSGraphicsContext*, SEL);
  void (*DPSinitclip)
        (NSGraphicsContext*, SEL);
  void (*DPSlineto__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPSmoveto__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPSnewpath)
        (NSGraphicsContext*, SEL);
  void (*DPSpathbbox____)
        (NSGraphicsContext*, SEL, float*, float*, float*, float*);
  void (*DPSrcurveto______)
        (NSGraphicsContext*, SEL, float, float, float, float, float, float);
  void (*DPSrectclip____)
        (NSGraphicsContext*, SEL, float, float, float, float);
  void (*DPSrectfill____)
        (NSGraphicsContext*, SEL, float, float, float, float);
  void (*DPSrectstroke____)
        (NSGraphicsContext*, SEL, float, float, float, float);
  void (*DPSreversepath)
        (NSGraphicsContext*, SEL);
  void (*DPSrlineto__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPSrmoveto__)
        (NSGraphicsContext*, SEL, float, float);
  void (*DPSstroke)
        (NSGraphicsContext*, SEL);

  void (*GSSendBezierPath_)
        (NSGraphicsContext*, SEL, NSBezierPath *);
  void (*GSRectClipList__)
        (NSGraphicsContext*, SEL, const NSRect *, int);
  void (*GSRectFillList__)
        (NSGraphicsContext*, SEL, const NSRect *, int);

/* ----------------------------------------------------------------------- */
/* Window system ops */
/* ----------------------------------------------------------------------- */
  void (*GSCurrentDevice___)
        (NSGraphicsContext*, SEL, void**, int*, int*);
  void (*DPScurrentoffset__)
        (NSGraphicsContext*, SEL, int*, int*);
  void (*GSSetDevice___)
        (NSGraphicsContext*, SEL, void*, int, int);
  void (*DPSsetoffset__)
        (NSGraphicsContext*, SEL, short int, short int);

/*-------------------------------------------------------------------------*/
/* Graphics Extensions Ops */
/*-------------------------------------------------------------------------*/
  void (*DPScomposite________)
        (NSGraphicsContext*, SEL, float, float, float, float, int, float, float, int);
  void (*DPScompositerect_____)
        (NSGraphicsContext*, SEL, float, float, float, float, int);
  void (*DPSdissolve________)
        (NSGraphicsContext*, SEL, float, float, float, float, int, float, float, float);

  void (*GSDrawImage__)
        (NSGraphicsContext*, SEL, NSRect, void *);

/* ----------------------------------------------------------------------- */
/* Postscript Client functions */
/* ----------------------------------------------------------------------- */
  void (*DPSPrintf__)
        (NSGraphicsContext*, SEL, const char *, va_list);
  void (*DPSWriteData__)
        (NSGraphicsContext*, SEL, const char *, unsigned int);

/* ----------------------------------------------------------------------- */
/* NSGraphics Ops */	
/* ----------------------------------------------------------------------- */
  NSDictionary * (*GSReadRect_)
        (NSGraphicsContext*, SEL, NSRect);

  void (*NSBeep)
        (NSGraphicsContext*, SEL);

/* Context helper wraps */
  void (*GSWSetViewIsFlipped_)
        (NSGraphicsContext*, SEL, BOOL);
  BOOL (*GSWViewIsFlipped)
        (NSGraphicsContext*, SEL);

/*
 * Render Bitmap Images
 */
  void (*NSDrawBitmap___________)(NSGraphicsContext*, SEL, NSRect rect,
                  int pixelsWide,
                  int pixelsHigh,
                  int bitsPerSample,
                  int samplesPerPixel,
                  int bitsPerPixel,
                  int bytesPerRow, 
                  BOOL isPlanar,
                  BOOL hasAlpha, 
                  NSString *colorSpaceName, 
                  const unsigned char *const data[5]);

  /* This probably belongs next to DPSstroke, but inserting members in this
  struct breaks apps that use PS* or DPS* functions and were compiled with
  an earlier version, so it's here until we figure out how to handle that.
  */
  void (*DPSshfill)
        (NSGraphicsContext*, SEL, NSDictionary *);
} gsMethodTable;

#endif
