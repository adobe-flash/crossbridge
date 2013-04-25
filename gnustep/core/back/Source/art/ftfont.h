/*
   Copyright (C) 2002 Free Software Foundation, Inc.

   Author:  Alexander Malmberg <alexander@malmberg.org>

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

#ifndef ftfont_h
#define ftfont_h

@class NSAffineTransform;

@protocol FTFontInfo
-(void) drawString: (const char *)s
	at: (int)x : (int)y
	to: (int)x0 : (int)y0 : (int)x1 : (int)y1
	: (unsigned char *)buf : (int)bpl
	: (unsigned char *)abuf : (int)abpl
	color: (unsigned char)r : (unsigned char)g : (unsigned char)b
	: (unsigned char)alpha
	transform: (NSAffineTransform *)transform
	deltas: (const float *)delta_data : (int)delta_size : (int)delta_flags
        widthChar: (int) wch
	drawinfo: (struct draw_info_s *)di;

-(void) drawGlyphs: (const NSGlyph *)glyphs : (int)length
	at: (int)x : (int)y
	to: (int)x0 : (int)y0 : (int)x1 : (int)y1
	: (unsigned char *)buf : (int)bpl
	color: (unsigned char)r : (unsigned char)g : (unsigned char)b
	: (unsigned char)alpha
	transform: (NSAffineTransform *)transform
	drawinfo: (struct draw_info_s *)di;

-(void) drawGlyphs: (const NSGlyph *)glyphs : (int)length
	at: (int)x : (int)y
	to: (int)x0 : (int)y0 : (int)x1 : (int)y1
	: (unsigned char *)buf : (int)bpl
	alpha: (unsigned char *)abuf : (int)abpl
	color: (unsigned char)r : (unsigned char)g : (unsigned char)b
	: (unsigned char)alpha
	transform: (NSAffineTransform *)transform
	drawinfo: (struct draw_info_s *)di;

-(void) outlineString: (const char *)s
	at: (float)x : (float)y
	gstate: (void *)func_param;

+(void) initializeBackend;
@end

@class FTFontInfo;

#endif

