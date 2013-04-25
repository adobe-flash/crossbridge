/** <title>NSStringAdditions</title>

   <abstract>Categories which add drawing capabilities to NSAttributedString
   and NSString.</abstract>

   Copyright (C) 1999, 2003, 2004 Free Software Foundation, Inc.

   Author: Richard Frith-Macdonald <richard@brainstorm.co.uk>
   Date: Mar 1999 - rewrite from scratch

   Author: Alexander Malmberg <alexander@malmberg.org>
   Date: November 2002 - February 2003 (rewrite to use NSLayoutManager et al)

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

#include <math.h>

#import <Foundation/NSException.h>
#import <Foundation/NSLock.h>

#import "AppKit/NSAffineTransform.h"
#import "AppKit/NSLayoutManager.h"
#import "AppKit/NSStringDrawing.h"
#import "AppKit/NSTextContainer.h"
#import "AppKit/NSTextStorage.h"
#import "AppKit/DPSOperators.h"

/*
TODO:

these methods are _not_ reentrant. should they be?
*/


#define LARGE_SIZE 4e6
/*
4e6 is chosen because it's close to (half of, for extra margin) 1<<23-1, the
largest number that can be stored in a 32-bit float with an ulp of 0.5, which
means things should round to the correct whole point.
*/


#define NUM_CACHE_ENTRIES 16
#define HIT_BOOST         2
#define MISS_COST         1
/*
A size of 16 and these constants give a hit rate of 80%-90% for normal app
use (based on real world statistics gathered with the help of some users
from #GNUstep).
*/


typedef struct
{
  int used;
  unsigned int string_hash;
  int hasSize, useScreenFonts;

  NSTextStorage *textStorage;
  NSLayoutManager *layoutManager;
  NSTextContainer *textContainer;

  NSSize givenSize;
  NSRect usedRect;
} cache_t;


static BOOL did_init;
static cache_t cache[NUM_CACHE_ENTRIES];

static NSTextStorage   *scratchTextStorage;
static NSLayoutManager *scratchLayoutManager;
static NSTextContainer *scratchTextContainer;

static NSRecursiveLock *cacheLock = nil;

static int total, hits, misses, hash_hits;

/* For collecting statistics. */
//#define STATS

#ifdef STATS
static void NSStringDrawing_dump_stats(void)
{
#define P(x) printf("%15i %s\n", x, #x);
  P(total)
  P(hits)
  P(misses)
  P(hash_hits)
#undef P
  printf("%15.8f hit ratio\n", hits / (double)total);
}
#endif


static void init_string_drawing(void)
{
  int i;
  NSTextStorage *textStorage;
  NSLayoutManager *layoutManager;
  NSTextContainer *textContainer;

  if (did_init)
    return;
  did_init = YES;

#ifdef STATS
  atexit(NSStringDrawing_dump_stats);
#endif
  
  for (i = 0; i < NUM_CACHE_ENTRIES + 1; i++)
    {
      textStorage = [[NSTextStorage alloc] init];
      layoutManager = [[NSLayoutManager alloc] init];
      [textStorage addLayoutManager: layoutManager];
      [layoutManager release];
      textContainer = [[NSTextContainer alloc]
			initWithContainerSize: NSMakeSize(10, 10)];
      [textContainer setLineFragmentPadding: 0];
      [layoutManager addTextContainer: textContainer];
      [textContainer release];
      
      if (i < NUM_CACHE_ENTRIES)
	{
	  cache[i].textStorage = textStorage;
	  cache[i].layoutManager = layoutManager;
	  cache[i].textContainer = textContainer;
	}
      else
	{
	  scratchTextStorage = textStorage;
	  scratchLayoutManager = layoutManager;
	  scratchTextContainer = textContainer;
	}
    }
}

static void cache_lock()
{
  if(cacheLock == nil)
    {
      cacheLock = [[NSRecursiveLock alloc] init];
    }
  [cacheLock lock];
}

static void cache_unlock()
{
  [cacheLock unlock];
}

static int cache_match(int hasSize, NSSize size, int useScreenFonts, int *matched)
{
  int i, j;
  cache_t *c;
  int least_used;
  int replace;
  int orig_used;

  unsigned int string_hash = [[scratchTextStorage string] hash];

  total++;

  *matched = 1;
  replace = least_used = -1;

  /*
  A deterministic pattern for replacing cache entries can hit ugly worst
  cases on certain matching use patterns (where the cache is full of old
  unused entries, but the new entries keep replacing each other).

  By starting at a random index, we avoid this kind of problem.
  */
  j = rand() % NUM_CACHE_ENTRIES;
  for (i = 0; i < NUM_CACHE_ENTRIES; i++, j++)
    {
      if (j == NUM_CACHE_ENTRIES)
	j = 0;
      c = cache + j;
      if (least_used == -1 || c->used < least_used)
	{
	  least_used = c->used;
	  replace = j;
	}

      if (!c->used)
	continue;

      orig_used = c->used;
      if (c->used > MISS_COST)
	c->used -= MISS_COST;
      else
	c->used = 1;

      if (c->string_hash != string_hash
	  || c->useScreenFonts != useScreenFonts)
	continue;

      hash_hits++;

      if (![scratchTextStorage isEqualToAttributedString: c->textStorage])
	continue;

      /* String and attributes match. */
      if (!c->hasSize && !hasSize)
	{
	  c->used = orig_used + HIT_BOOST;
	  hits++;
	  return j;
	}

      if (c->hasSize && hasSize && c->givenSize.width == size.width
	  && c->givenSize.height == size.height)
	{
	  c->used = orig_used + HIT_BOOST;
	  hits++;
	  return j;
	}

      if (!c->hasSize && hasSize && size.width >= NSMaxX(c->usedRect)
	  && size.height >= NSMaxY(c->usedRect))
	{
	  c->used = orig_used + HIT_BOOST;
	  hits++;
	  return j;
	}
    }

  NSCAssert(replace != -1, @"Couldn't find a cache entry to replace.");

  misses++;
  *matched = 0;

  c = cache + replace;
  c->used = 1;
  c->string_hash = string_hash;
  c->hasSize = hasSize;
  c->useScreenFonts = useScreenFonts;
  c->givenSize = size;

  {
    id temp;

#define SWAP(a, b) temp = a; a = b; b = temp;
    SWAP(scratchTextStorage, c->textStorage)
    SWAP(scratchLayoutManager, c->layoutManager)
    SWAP(scratchTextContainer, c->textContainer)
#undef SWAP
  }

  return replace;
}



static int cache_lookup_string(NSString *string, NSDictionary *attributes,
	int hasSize, NSSize size, int useScreenFonts)
{
  cache_t *c;
  int ci, hit;
  NSLayoutManager *layoutManager;
  NSTextContainer *textContainer;

  if (!did_init)
    init_string_drawing();

  /*
    This is a hack, but it's an efficient way of getting the layout manager
    to just ditch all old information, and here, we don't want it to try to
    be clever and cache or soft-invalidate anything since the new string has
    nothing to do with the old one.
    
    TODO: the layout manager should realize this by itself
  */
  [scratchLayoutManager setTextStorage: scratchTextStorage];
  
  [scratchTextStorage beginEditing];
  [scratchTextStorage replaceCharactersInRange: NSMakeRange(0, [scratchTextStorage length])
		      withString: @""];
  if ([string length])
    {
      [scratchTextStorage replaceCharactersInRange: NSMakeRange(0, 0)
			  withString: string];
      [scratchTextStorage setAttributes: attributes
			  range: NSMakeRange(0, [string length])];
    }
  [scratchTextStorage endEditing];
  
  ci = cache_match(hasSize, size, useScreenFonts, &hit);
  
  if (hit)
    {
      return ci;
    }
  c = &cache[ci];
  
  layoutManager = c->layoutManager;
  textContainer = c->textContainer;
  
  if (hasSize)
    [textContainer setContainerSize: NSMakeSize(size.width, LARGE_SIZE)];
  else
    [textContainer setContainerSize: NSMakeSize(LARGE_SIZE, LARGE_SIZE)];
  [layoutManager setUsesScreenFonts: useScreenFonts];
  
  c->usedRect = [layoutManager usedRectForTextContainer: textContainer];

  return ci;
}

static int cache_lookup_attributed_string(NSAttributedString *string,
	int hasSize, NSSize size, int useScreenFonts)
{
  cache_t *c;
  int ci, hit;
  NSLayoutManager *layoutManager;
  NSTextContainer *textContainer;

  if (!did_init)
    init_string_drawing();
  
  [scratchTextStorage replaceCharactersInRange: NSMakeRange(0, [scratchTextStorage length])
		      withString: @""];
  [scratchTextStorage replaceCharactersInRange: NSMakeRange(0, 0)
		      withAttributedString: string];
  
  ci = cache_match(hasSize, size, useScreenFonts, &hit);
  
  if (hit)
    {
      return ci;
    }
  
  c = &cache[ci];
  
  layoutManager = c->layoutManager;
  textContainer = c->textContainer;
  
  if (hasSize)
    [textContainer setContainerSize: NSMakeSize(size.width, LARGE_SIZE)];
  else
    [textContainer setContainerSize: NSMakeSize(LARGE_SIZE, LARGE_SIZE)];
  [layoutManager setUsesScreenFonts: useScreenFonts];
  
  c->usedRect = [layoutManager usedRectForTextContainer: textContainer];

  return ci;
}


static int use_screen_fonts(void)
{
  NSGraphicsContext		*ctxt = GSCurrentContext();
  NSAffineTransform		*ctm = GSCurrentCTM(ctxt);
  NSAffineTransformStruct	ts = [ctm transformStruct];

  if (ts.m11 != 1.0 || ts.m12 != 0.0 || ts.m21 != 0.0 || fabs(ts.m22) != 1.0)
    {
      return 0;
    }
  else
    {
      return 1;
    }
}

/*
This is an ugly hack to get text to display correctly in non-flipped views.

The text system always has positive y down, so we flip the coordinate
system when drawing (if the view isn't flipped already). This causes the
glyphs to be drawn upside-down, so we need to tell NSFont to flip the fonts.
*/
@interface NSFont (FontFlipHack)
+(void) _setFontFlipHack: (BOOL)flip;
@end


@implementation NSAttributedString (NSStringDrawing)

- (void) drawAtPoint: (NSPoint)point
{
  int ci;
  cache_t *c;

  NSRange r;
  NSGraphicsContext *ctxt = GSCurrentContext();

  cache_lock();

  NS_DURING
    {
      ci = cache_lookup_attributed_string(self, 0, NSZeroSize, use_screen_fonts());
      c = &cache[ci];
      
      r = NSMakeRange(0, [c->layoutManager numberOfGlyphs]);
      
      if (![[NSView focusView] isFlipped])
	{
	  DPSscale(ctxt, 1, -1);
	  point.y = -point.y;
	  
	  /*
	    Adjust point.y so the lower left corner of the used rect is at the
	    point that was passed to us.
	  */
	  point.y -= NSMaxY(c->usedRect);
	  
	  [NSFont _setFontFlipHack: YES];
	}
      
      [c->layoutManager drawBackgroundForGlyphRange: r
	atPoint: point];
      
      [c->layoutManager drawGlyphsForGlyphRange: r
	atPoint: point];
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  if (![[NSView focusView] isFlipped])
    {
      DPSscale(ctxt, 1, -1);
      [NSFont _setFontFlipHack: NO];
    }
}

- (void) drawInRect: (NSRect)rect
{
  int ci;
  cache_t *c;

  NSRange r;
  BOOL need_clip = NO;
  NSGraphicsContext *ctxt = GSCurrentContext();

  if (rect.size.width <= 0 || rect.size.height <= 0)
    return;
      

  cache_lock();

  NS_DURING
    {
      ci = cache_lookup_attributed_string(self, 1, rect.size, use_screen_fonts());
      c = &cache[ci];
      
      /*
	If the used rect fits completely in the rect we draw in, we save time
	by avoiding the DPSrectclip (and the state save and restore).
	
	This isn't completely safe; the used rect isn't guaranteed to contain
	all parts of all glyphs.
      */
      if (c->usedRect.origin.x >= 0 && c->usedRect.origin.y <= 0
	  && NSMaxX(c->usedRect) <= rect.size.width
	  && NSMaxY(c->usedRect) <= rect.size.height)
	{
	  need_clip = NO;
	}
      else
	{
	  need_clip = YES;
	  DPSgsave(ctxt);
	  DPSrectclip(ctxt, rect.origin.x, rect.origin.y,
		      rect.size.width, rect.size.height);
	}
      
      r = [c->layoutManager
	    glyphRangeForBoundingRect: NSMakeRect(0, 0, rect.size.width,
						  rect.size.height)
	    inTextContainer: c->textContainer];
      
      if (![[NSView focusView] isFlipped])
	{
	  DPSscale(ctxt, 1, -1);
	  rect.origin.y = -NSMaxY(rect);
	  [NSFont _setFontFlipHack: YES];
	}
      
      [c->layoutManager drawBackgroundForGlyphRange: r
	atPoint: rect.origin];
      
      [c->layoutManager drawGlyphsForGlyphRange: r
	atPoint: rect.origin];
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  [NSFont _setFontFlipHack: NO];
  if (![[NSView focusView] isFlipped])
    {
      DPSscale(ctxt, 1, -1);
    }

  if (need_clip)
    {
      /* Restore the original clipping path. */
      DPSgrestore(ctxt);
    }
}

- (void) drawWithRect:(NSRect)rect
              options:(NSStringDrawingOptions)options
{
  // FIXME: This ignores options
  [self drawInRect: rect];
}

- (NSSize) size
{
  int ci;
  NSSize result = NSZeroSize;

  cache_lock();
  NS_DURING
    {
      ci = cache_lookup_attributed_string(self, 0, NSZeroSize, 1);
      /*
	An argument could be made for using NSMaxX/NSMaxY here, but that fails
	horribly on right-aligned strings. For now, we handle that case in a
	useful way and ignore indents.
      */
      
      result = cache[ci].usedRect.size; 
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  return result;
}

- (NSRect) boundingRectWithSize: (NSSize)size
                        options: (NSStringDrawingOptions)options
{
  int ci;
  NSRect result = NSZeroRect;

  cache_lock();
  NS_DURING
    {    
      // FIXME: This ignores options
      ci = cache_lookup_attributed_string(self, 1, size, 1);
      /*
	An argument could be made for using NSMaxX/NSMaxY here, but that fails
	horribly on right-aligned strings. For now, we handle that case in a
	useful way and ignore indents.
      */
      
      result = cache[ci].usedRect;
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  return result;
}

@end

/*
TODO: It's severely sub-optimal, but the NSString methods just use
NSAttributedString to do the job.
*/
@implementation NSString (NSStringDrawing)

- (void) drawAtPoint: (NSPoint)point withAttributes: (NSDictionary *)attrs
{
  int ci;
  cache_t *c;

  NSRange r;
  NSGraphicsContext *ctxt = GSCurrentContext();

  cache_lock();
  NS_DURING
    {
      ci = cache_lookup_string(self, attrs, 0, NSZeroSize, use_screen_fonts());
      c = &cache[ci];
      
      r = NSMakeRange(0, [c->layoutManager numberOfGlyphs]);
      
      if (![[NSView focusView] isFlipped])
	{
	  DPSscale(ctxt, 1, -1);
	  point.y = -point.y;
	  
	  /*
	    Adjust point.y so the lower left corner of the used rect is at the
	    point that was passed to us.
	  */
	  point.y -= NSMaxY(c->usedRect);
	  
	  [NSFont _setFontFlipHack: YES];
	}
      
      [c->layoutManager drawBackgroundForGlyphRange: r
	atPoint: point];
      
      [c->layoutManager drawGlyphsForGlyphRange: r
	atPoint: point];
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  if (![[NSView focusView] isFlipped])
    {
      DPSscale(ctxt, 1, -1);
      [NSFont _setFontFlipHack: NO];
    }
}

- (void) drawInRect: (NSRect)rect withAttributes: (NSDictionary *)attrs
{
  int ci;
  cache_t *c;

  NSRange r;
  BOOL need_clip = NO;
  NSGraphicsContext *ctxt = GSCurrentContext();

  if (rect.size.width <= 0 || rect.size.height <= 0)
    return;
  
  cache_lock();
  NS_DURING
    {    
      ci = cache_lookup_string(self, attrs, 1, rect.size, use_screen_fonts());
      c = &cache[ci];
      
      /*
	If the used rect fits completely in the rect we draw in, we save time
	by avoiding the DPSrectclip (and the state save and restore).
	
	This isn't completely safe; the used rect isn't guaranteed to contain
	all parts of all glyphs.
      */
      if (c->usedRect.origin.x >= 0 && c->usedRect.origin.y <= 0
	  && NSMaxX(c->usedRect) <= rect.size.width
	  && NSMaxY(c->usedRect) <= rect.size.height)
	{
	  need_clip = NO;
	}
      else
	{
	  need_clip = YES;
	  DPSgsave(ctxt);
	  DPSrectclip(ctxt, rect.origin.x, rect.origin.y,
		      rect.size.width, rect.size.height);
	}
      
      r = [c->layoutManager
	    glyphRangeForBoundingRect: NSMakeRect(0, 0, rect.size.width,
						  rect.size.height)
	    inTextContainer: c->textContainer];
      
      if (![[NSView focusView] isFlipped])
	{
	  DPSscale(ctxt, 1, -1);
	  rect.origin.y = -NSMaxY(rect);
	  [NSFont _setFontFlipHack: YES];
	}
      
      [c->layoutManager drawBackgroundForGlyphRange: r
	atPoint: rect.origin];
      
      [c->layoutManager drawGlyphsForGlyphRange: r
	atPoint: rect.origin];      
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();
      
  [NSFont _setFontFlipHack: NO];
  if (![[NSView focusView] isFlipped])
    {
      DPSscale(ctxt, 1, -1);
    }

  if (need_clip)
    {
      /* Restore the original clipping path. */
      DPSgrestore(ctxt);
    }
}

- (void) drawWithRect: (NSRect)rect
              options: (NSStringDrawingOptions)options
           attributes: (NSDictionary *)attributes
{
  // FIXME: This ignores options
  [self drawInRect: rect withAttributes: attributes];
}

- (NSSize) sizeWithAttributes: (NSDictionary *)attrs
{
  int ci;
  NSSize result = NSZeroSize;

  cache_lock();
  NS_DURING
    {
      ci = cache_lookup_string(self, attrs, 0, NSZeroSize, 1);
      /*
	An argument could be made for using NSMaxX/NSMaxY here, but that fails
	horribly on right-aligned strings (which may be the right thing). For now,
	we handle that case in a useful way and ignore indents.
      */
      
      result = cache[ci].usedRect.size;
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  return result;
}

- (NSRect) boundingRectWithSize: (NSSize)size
                        options: (NSStringDrawingOptions)options
                     attributes: (NSDictionary *)attributes
{
  int ci;
  NSRect result = NSZeroRect;

  cache_lock();
  NS_DURING
    {
      // FIXME: This ignores options
      ci = cache_lookup_string(self, attributes, 1, size, 1);
      /*
	An argument could be made for using NSMaxX/NSMaxY here, but that fails
	horribly on right-aligned strings (which may be the right thing). For now,
	we handle that case in a useful way and ignore indents.
      */
      
      result = cache[ci].usedRect;
    }
  NS_HANDLER
    {
      cache_unlock();
      [localException raise];
    }
  NS_ENDHANDLER;
  cache_unlock();

  return result;
}

@end


/*
Dummy function; see comment in NSApplication.m, +initialize.
*/
void GSStringDrawingDummyFunction(void)
{
}

