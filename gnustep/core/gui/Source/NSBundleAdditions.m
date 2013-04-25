/** <title>NSBundleAdditions</title>

   <abstract>Implementation of NSBundle Additions</abstract>

   Copyright (C) 1997, 1999 Free Software Foundation, Inc.

   Author:  Simon Frankau <sgf@frankau.demon.co.uk>
   Date: 1997
   Author:  Richard Frith-Macdonald <richard@brainstorm.co.uk>
   Date: 1999
   Author:  Gregory John Casamento <greg_casamento@yahoo.com>
   Date: 2000
   
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

#import "config.h"
#import <Foundation/NSArray.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSCoder.h>
#import <Foundation/NSDictionary.h>
#import <Foundation/NSEnumerator.h>
#import <Foundation/NSException.h>
#import <Foundation/NSString.h>
#import <Foundation/NSURL.h>
#import <Foundation/NSUserDefaults.h>
#import <Foundation/NSKeyValueCoding.h>
#import "AppKit/NSControl.h"
#import "AppKit/NSNib.h"
#import "AppKit/NSNibConnector.h"
#import "AppKit/NSNibLoading.h"
#import "GNUstepGUI/GSModelLoaderFactory.h"

@implementation	NSNibConnector

- (void) dealloc
{
  RELEASE(_src);
  RELEASE(_dst);
  RELEASE(_tag);
  [super dealloc];
}

- (id) destination
{
  return _dst;
}

- (void) encodeWithCoder: (NSCoder*)aCoder
{
  if ([aCoder allowsKeyedCoding])
    {
      if (_src != nil)
	{
	  [aCoder encodeObject: _src forKey: @"NSSource"];
	}
      if (_dst != nil)
	{
	  [aCoder encodeObject: _dst forKey: @"NSDestination"];
	}
      if (_tag != nil)
	{
	  [aCoder encodeObject: _tag forKey: @"NSLabel"];
	}
    }
  else
    {
      [aCoder encodeObject: _src];
      [aCoder encodeObject: _dst];
      [aCoder encodeObject: _tag];
    }
}

- (void) establishConnection
{
}

- (id) initWithCoder: (NSCoder*)aDecoder
{
  if ([aDecoder allowsKeyedCoding])
    {
      if ([aDecoder containsValueForKey: @"NSDestination"])
	{
	  ASSIGN(_dst, [aDecoder decodeObjectForKey: @"NSDestination"]);
	}
      if ([aDecoder containsValueForKey: @"NSSource"])
	{
	  ASSIGN(_src, [aDecoder decodeObjectForKey: @"NSSource"]);
	}
      if ([aDecoder containsValueForKey: @"NSLabel"])
	{      
	  ASSIGN(_tag, [aDecoder decodeObjectForKey: @"NSLabel"]);
	}
    }
  else
    {
      [aDecoder decodeValueOfObjCType: @encode(id) at: &_src];
      [aDecoder decodeValueOfObjCType: @encode(id) at: &_dst];
      [aDecoder decodeValueOfObjCType: @encode(id) at: &_tag];
    }
  return self;
}

- (NSString*) label
{
  return _tag;
}

- (void) replaceObject: (id)anObject withObject: (id)anotherObject
{
  if (_src == anObject)
    {
      ASSIGN(_src, anotherObject);
    }
  if (_dst == anObject)
    {
      ASSIGN(_dst, anotherObject);
    }
  if (_tag == anObject)
    {
      ASSIGN(_tag, anotherObject);
    }
}

- (id) source
{
  return _src;
}

- (void) setDestination: (id)anObject
{
  ASSIGN(_dst, anObject);
}

- (void) setLabel: (NSString*)label
{
  ASSIGN(_tag, label);
}

- (void) setSource: (id)anObject
{
  ASSIGN(_src, anObject);
}

- (NSString *)description
{
  NSString *desc = [NSString stringWithFormat: @"<%@ src=%@ dst=%@ label=%@>",
			     [super description],
			     [self source],
			     [self destination],
			     [self label]];
  return desc;
}
@end

@implementation	NSNibControlConnector
- (void) establishConnection
{
  SEL sel = NSSelectorFromString(_tag);
	      
  [_src setTarget: _dst];
  [_src setAction: sel];
}
@end

@implementation	NSNibOutletConnector
- (void) establishConnection
{
  NS_DURING
    {
      if (_src != nil)
	{
          NSString *selName;
          SEL sel; 	 
          
          selName = [NSString stringWithFormat: @"set%@%@:", 	 
                       [[_tag substringToIndex: 1] uppercaseString], 	 
                      [_tag substringFromIndex: 1]]; 	 
          sel = NSSelectorFromString(selName); 	 
          
          if (sel && [_src respondsToSelector: sel]) 	 
            { 	 
              [_src performSelector: sel withObject: _dst]; 	 
            } 	 
          else 	 
            { 	 
              const char *nam = [_tag cString]; 	 
              const char *type; 	 
              unsigned int size; 	 
              int offset; 	 
              
              /* 	 
               * Use the GNUstep additional function to set the instance 	 
               * variable directly. 	 
               * FIXME - need some way to do this for libFoundation and 	 
               * Foundation based systems. 	 
               */ 	 
              if (GSObjCFindVariable(_src, nam, &type, &size, &offset)) 	 
                { 	 
                  GSObjCSetVariable(_src, offset, size, (void*)&_dst); 	 
                } 	 
            }
	}
    }
  NS_HANDLER
    {
      NSLog(@"Error while establishing connection %@: %@",self,[localException reason]);
    }
  NS_ENDHANDLER;
}
@end

@implementation NSBundle (NSBundleAdditions)
+ (BOOL) loadNibFile: (NSString*)fileName
   externalNameTable: (NSDictionary*)context
	    withZone: (NSZone*)zone
{
  NSNib *nib = [[NSNib alloc] initWithContentsOfURL: [NSURL fileURLWithPath: fileName]];
  BOOL loaded = [nib instantiateNibWithExternalNameTable: context
                                                withZone: zone];

  RELEASE(nib);
  return loaded;
}

+ (BOOL) loadNibNamed: (NSString *)aNibName
	        owner: (id)owner
{
  NSDictionary	*table;
  NSBundle	*bundle;

  if (owner == nil || aNibName == nil)
    {
      return NO;
    }
  table = [NSDictionary dictionaryWithObject: owner forKey: NSNibOwner];

  /*
   * First look for the NIB in the bundle corresponding to the owning class,
   * since the class may have been loaded dynamically and the bundle may
   * contain class-specific NIB resources as well as code.
   * If that fails, try to load the NIB from the main application bundle,
   * which is where most NIB resources are to be found.
   * Possibly this is the wrong order ... since it's conceivable that an
   * application may supply an alternative NIB which it would like to have
   * used in preference to the one in the classes bundle.  However I could
   * not find the behavior documented anywhere and the current order is
   * most consistent with the the way the code behaved before I changed it.
   */
  bundle = [self bundleForClass: [owner class]];
  if (bundle != nil && [bundle loadNibFile: aNibName
			 externalNameTable: table
				  withZone: [owner zone]] == YES)
    {
      return YES;
    }
  else
    {
      bundle = [self mainBundle];
    }
  return [bundle loadNibFile: aNibName
	   externalNameTable: table
		    withZone: [owner zone]];
}

- (NSString *) pathForNibResource: (NSString *)fileName
{
  NSMutableArray	*array = [NSMutableArray arrayWithCapacity: 8];
  NSArray		*languages;
  NSString		*rootPath = [self bundlePath];
  NSString		*primary;
  NSString		*language;
  NSEnumerator		*enumerator;

  languages = [[NSUserDefaults standardUserDefaults]
    stringArrayForKey: @"NSLanguages"];

  /*
   * Build an array of resource paths that differs from the normal order -
   * we want a localized file in preference to a generic one.
   */
  primary = [rootPath stringByAppendingPathComponent: @"Resources"];
  enumerator = [languages objectEnumerator];
  while ((language = [enumerator nextObject]))
    {
      NSString	*langDir;

      langDir = [NSString stringWithFormat: @"%@.lproj", language];
      [array addObject: [primary stringByAppendingPathComponent: langDir]];
    }
  [array addObject: primary];
  primary = rootPath;
  enumerator = [languages objectEnumerator];
  while ((language = [enumerator nextObject]))
    {
      NSString	*langDir;

      langDir = [NSString stringWithFormat: @"%@.lproj", language];
      [array addObject: [primary stringByAppendingPathComponent: langDir]];
    }
  [array addObject: primary];

  enumerator = [array objectEnumerator];
  while ((rootPath = [enumerator nextObject]) != nil)
    {
      NSString *modelPath = [rootPath stringByAppendingPathComponent: fileName];
      NSString *path = [GSModelLoaderFactory supportedModelFileAtPath: modelPath];
      
      if (path != nil)
	{
	  return path;
	}
    }

  return nil;
}

- (BOOL) loadNibFile: (NSString*)fileName
   externalNameTable: (NSDictionary*)context
	    withZone: (NSZone*)zone
{
  NSString *path = [self pathForNibResource: fileName];

  if (path != nil)
    {
      return [NSBundle loadNibFile: path
		 externalNameTable: context
			  withZone: (NSZone*)zone];
    }
  else 
    {
      return NO;
    }
}
@end
// end of NSBundleAdditions
