/* GSGStateOPS - Ops for GSGState

   Copyright (C) 1998 Free Software Foundation, Inc.

   Written by:  Adam Fedor <fedor@gnu.org>
   Date: Nov 1998
   
   This file is part of the GNU Objective C User Interface Library.

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

#ifndef _GSGStateOps_h_INCLUDE
#define _GSGStateOps_h_INCLUDE

@class NSGradient;

@interface GSGState (Ops)
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
- (void) GSSetFont: (GSFontInfo *)fontref;
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
/* Gstate operations */
/* ----------------------------------------------------------------------- */
- (void) DPSinitgraphics;

- (void) DPScurrentflat: (float*)flatness;
- (void) DPScurrentlinecap: (int*)linecap;
- (void) DPScurrentlinejoin: (int*)linejoin;
- (void) DPScurrentlinewidth: (float*)width;
- (void) DPScurrentmiterlimit: (float*)limit;
- (void) DPScurrentpoint: (float*)x : (float*)y;
- (void) DPScurrentstrokeadjust: (int*)b;
- (void) DPSsetdash: (const float*)pat : (int)size : (float)offset;
- (void) DPSsetflat: (float)flatness;
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
- (NSPoint) currentPoint;

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

- (void) GSSendBezierPath: (NSBezierPath *)path;
- (void) GSRectClipList: (const NSRect *)rects : (int) count;
- (void) GSRectFillList: (const NSRect *)rects : (int) count;

- (NSDictionary *) GSReadRect: (NSRect)rect;

- (void)DPSimage: (NSAffineTransform*) matrix 
		: (int) pixelsWide : (int) pixelsHigh
		: (int) bitsPerSample : (int) samplesPerPixel 
		: (int) bitsPerPixel : (int) bytesPerRow : (BOOL) isPlanar
		: (BOOL) hasAlpha : (NSString *) colorSpaceName
		: (const unsigned char *const [5]) data;

- (void) DPSshfill: (NSDictionary *)shader;

@end

@interface GSGState (PatternColor)

- (void *) saveClip;
- (void) restoreClip: (void *)savedClip;
- (void) fillRect: (NSRect)rect withPattern: (NSImage*)pattern;
- (void) fillPath: (NSBezierPath*)fillPath withPattern: (NSImage*)pattern;
- (void) eofillPath: (NSBezierPath*)fillPath withPattern: (NSImage*)pattern;

@end

@interface GSGState (NSGradient)

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

#endif
