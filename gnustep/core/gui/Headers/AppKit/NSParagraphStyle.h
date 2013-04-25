/* 
   NSParagraphStyle.h

     NSParagraphStyle and NSMutableParagraphStyle hold paragraph style 
     information NSTextTab holds information about a single tab stop

   Copyright (C) 1996,1999 Free Software Foundation, Inc.

   Author:  Daniel Böhringer <boehring@biomed.ruhr-uni-bochum.de>
   Date: August 1998
   Update: Richard Frith-Macdonald <richard@brainstorm.co.uk> March 1999 
   
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

#ifndef _GNUstep_H_NSParagraphStyle
#define _GNUstep_H_NSParagraphStyle
#import <GNUstepBase/GSVersionMacros.h>

#import <Foundation/NSObject.h>
#import <AppKit/NSText.h>

typedef enum _NSTextTabType {
  NSLeftTabStopType = 0,
  NSRightTabStopType,
  NSCenterTabStopType,
  NSDecimalTabStopType
} NSTextTabType;

typedef enum _NSLineBreakMode {		/* What to do with long lines */
  NSLineBreakByWordWrapping = 0,     	/* Wrap at word boundaries, default */
  NSLineBreakByCharWrapping,		/* Wrap at character boundaries */
  NSLineBreakByClipping,		/* Simply clip */
  NSLineBreakByTruncatingHead,	/* Truncate at head of line: "...wxyz" */
  NSLineBreakByTruncatingTail,	/* Truncate at tail of line: "abcd..." */
  NSLineBreakByTruncatingMiddle	/* Truncate middle of line:  "ab...yz" */
} NSLineBreakMode;

typedef enum _NSWritingDirection {
    NSWritingDirectionNaturalDirection,
    NSWritingDirectionLeftToRight,
    NSWritingDirectionRightToLeft
} NSWritingDirection;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
APPKIT_EXPORT NSString *NSTabColumnTerminatorsAttributeName; 
#endif

@interface NSTextTab : NSObject <NSCopying, NSCoding>
{
  NSTextTabType	_tabStopType;
  NSDictionary *_options;
  NSTextAlignment _alignment;
  float	_location;
}

- (id) initWithType: (NSTextTabType)type  location: (float)loc;
- (float) location;
- (NSTextTabType) tabStopType;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
- (id) initWithTextAlignment: (NSTextAlignment)align 
                    location: (float)loc 
                     options: (NSDictionary *)options;
- (NSTextAlignment) alignment;
- (NSDictionary *) options;
#endif

@end

@interface NSParagraphStyle : NSObject <NSCopying, NSMutableCopying, NSCoding>
{
  NSMutableArray *_tabStops;
  NSArray *_textBlocks;
  NSArray *_textLists;
  float _lineSpacing;
  float _paragraphSpacing;
  float _headIndent;
  float _tailIndent;
  float _firstLineHeadIndent;
  float _minimumLineHeight;
  float _maximumLineHeight;
  float _paragraphSpacingBefore;
  float _defaultTabInterval;
  float _hyphenationFactor;
  float _lineHeightMultiple;
  float _tighteningFactorForTruncation;
  NSTextAlignment _alignment;
  NSLineBreakMode _lineBreakMode;
  NSWritingDirection _baseDirection;
  int _headerLevel;
}

+ (NSParagraphStyle*) defaultParagraphStyle;

/*
 *	"Leading": distance between the bottom of one line fragment and top
 *	of next (applied between lines in the same container).
 *	Can't be negative. This value is included in the line fragment
 *	heights in layout manager.
 */
- (float) lineSpacing;

/*
 *	Distance between the bottom of this paragraph and top of next.
 */
- (float) paragraphSpacing;

- (NSTextAlignment) alignment;

/*
 *	The following values are relative to the appropriate margin
 *	(depending on the paragraph direction)
 */

/*
 *	Distance from margin to front edge of paragraph
 */
- (float) headIndent;

/*
 *	Distance from margin to back edge of paragraph; if negative or 0,
 *	from other margin
 */
- (float) tailIndent;

/*
 *	Distance from margin to edge appropriate for text direction
 */
- (float) firstLineHeadIndent;

/*
 *	Distance from margin to tab stops
 */
- (NSArray *)tabStops;

/*
 *	Line height is the distance from bottom of descenders to to
 *	of ascenders; basically the line fragment height. Does not include
 *	lineSpacing (which is added after this computation).
 */
- (float) minimumLineHeight;

/*
 *	0 implies no maximum.
 */ 
- (float) maximumLineHeight;
- (NSLineBreakMode) lineBreakMode;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_2, GS_API_LATEST)
/*
 *      Returns the writing direction of "language", which is an ISO 639
 *      two- or three letter code, e.g. "en", or an ISO language-region
 *      format, e.g. "en_GB"
 */
+ (NSWritingDirection) defaultWritingDirectionForLanguage: (NSString*) language;
- (NSWritingDirection) baseWritingDirection;
#endif
#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
- (float) defaultTabInterval;
- (float) lineHeightMultiple;
- (float) paragraphSpacingBefore;
#endif
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
- (int) headerLevel;
- (float) hyphenationFactor;
- (NSArray *) textBlocks;
- (NSArray *) textLists;
- (float) tighteningFactorForTruncation;
#endif

@end

@interface NSMutableParagraphStyle : NSParagraphStyle
{
}

- (void) setLineSpacing: (float)aFloat;
- (void) setParagraphSpacing: (float)aFloat;
- (void) setAlignment: (NSTextAlignment)newAlignment;
- (void) setFirstLineHeadIndent: (float)aFloat;
- (void) setHeadIndent: (float)aFloat;
- (void) setTailIndent: (float)aFloat;
- (void) setLineBreakMode: (NSLineBreakMode)mode;
- (void) setMinimumLineHeight: (float)aFloat;
- (void) setMaximumLineHeight: (float)aFloat;
- (void) addTabStop: (NSTextTab*)anObject;
- (void) removeTabStop: (NSTextTab*)anObject;
- (void) setTabStops: (NSArray*)array;
- (void) setParagraphStyle: (NSParagraphStyle*)obj;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_2, GS_API_LATEST)
- (void) setBaseWritingDirection: (NSWritingDirection)direction;
#endif
#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
- (void) setDefaultTabInterval: (float)interval;
- (void) setLineHeightMultiple: (float)factor;
- (void) setParagraphSpacingBefore: (float)spacing;
#endif
#if OS_API_VERSION(MAC_OS_X_VERSION_10_4, GS_API_LATEST)
- (void) setHeaderLevel: (int)level;
- (void) setHyphenationFactor: (float)factor;
- (void) setTextBlocks: (NSArray *)blocks;
- (void) setTextLists: (NSArray *)lists;
- (void) setTighteningFactorForTruncation: (float)factor;
#endif

@end

#endif // _GNUstep_H_NSParagraphStyle
