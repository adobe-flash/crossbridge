/** <title>NSFileWrapper</title>

   <abstract>Hold a file's contents in dynamic memory.</abstract>

   Copyright (C) 1996 Free Software Foundation, Inc.

   Author: Felipe A. Rodriguez <far@ix.netcom.com>
   Date: Sept 1998
   Author: Jonathan Gapen <jagapen@whitewater.chem.wisc.edu>
   Date: Dec 1999
   
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

#include "config.h"

#import <Foundation/NSArchiver.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSData.h>
#import <Foundation/NSDebug.h>
#import <Foundation/NSException.h>
#import <Foundation/NSFileManager.h>
#import <Foundation/NSValue.h>
#import "AppKit/NSFileWrapper.h"
#import "AppKit/NSFont.h"
#import "AppKit/NSWorkspace.h"

@implementation NSFileWrapper

//
// Initialization 
//

// Init instance of directory type
- (id) initDirectoryWithFileWrappers: (NSDictionary*)docs
{
  self = [super init];
  if (self != nil)
    {
      NSEnumerator	*enumerator;
      id		key;
      NSFileWrapper	*wrapper;

      _wrapperType = GSFileWrapperDirectoryType;
      _wrapperData
	= [[NSMutableDictionary alloc] initWithCapacity: [docs count]];

      enumerator = [docs keyEnumerator];
      while ((key = [enumerator nextObject]) != nil)
	{
	  wrapper = (NSFileWrapper*)[docs objectForKey: key];

	  if (![wrapper preferredFilename])
	    {
	      [wrapper setPreferredFilename: key];
	    }
	  [_wrapperData setObject: wrapper forKey: key];
	}
    }
  return self;
}
    
// Init instance of regular file type
- (id) initRegularFileWithContents: (NSData*)data
{
  self = [super init];
  if (self != nil)
    {
      _wrapperData = [data copyWithZone: [self zone]];
      _wrapperType = GSFileWrapperRegularFileType;
     } 
  return self;
}

// Init instance of symbolic link type
- (id) initSymbolicLinkWithDestination: (NSString*)path
{
  self = [super init];
  if (self != nil)
    {
      _wrapperData = [path copyWithZone: [self zone]];
      _wrapperType = GSFileWrapperSymbolicLinkType;
    }
  return self;
}

/**
 * Init an instance from the file, directory, or symbolic link at path.<br /> 
 * This can create a tree of instances with a directory instance at the top
 */
- (id) initWithPath: (NSString*)path
{
  CREATE_AUTORELEASE_POOL(arp);
  NSFileManager	*fm = [NSFileManager defaultManager];
  NSString	*fileType;

  NSDebugLLog(@"NSFileWrapper", @"initWithPath: %@", path);

  // Store the full path in filename, the specification is unclear in this point
  [self setFilename: path];
  [self setPreferredFilename: [path lastPathComponent]];
  [self setFileAttributes: [fm fileAttributesAtPath: path traverseLink: NO]];

  fileType = [[self fileAttributes] fileType];
  if ([fileType isEqualToString: @"NSFileTypeDirectory"])
    {
      NSString		*filename;
      NSMutableArray	*fileWrappers = [NSMutableArray array];
      NSArray		*filenames = [fm directoryContentsAtPath: path];
      NSEnumerator	*enumerator = [filenames objectEnumerator];

      while ((filename = [enumerator nextObject]) != nil)
        {
	  NSFileWrapper	*w;

	  w = [[NSFileWrapper alloc] initWithPath: 
	       [path stringByAppendingPathComponent: filename]];
          [fileWrappers addObject: w]; 
	  RELEASE(w);
        }
      self = [self initDirectoryWithFileWrappers: 
        [NSDictionary dictionaryWithObjects: fileWrappers forKeys: filenames]];
    }
  else if ([fileType isEqualToString: @"NSFileTypeRegular"])
    {
      self = [self initRegularFileWithContents: 
		AUTORELEASE([[NSData alloc] initWithContentsOfFile: path])];
    }
  else if ([fileType isEqualToString: @"NSFileTypeSymbolicLink"])
    {
      self = [self initSymbolicLinkWithDestination: 
                 [fm pathContentOfSymbolicLinkAtPath: path]];
    }
  [arp drain];
  return self;
}

// Init an instance from data in std serial format.  Serial format is the
// same as that used by NSText's RTFDFromRange: method.  This can 
// create a tree of instances with a directory instance at the top
- (id) initWithSerializedRepresentation: (NSData*)data
{
  // FIXME - This should use a serializer. To get that working a helper object 
  // is needed that implements the NSObjCTypeSerializationCallBack protocol.
  // We should add this later, currently the NSArchiver is used.
  // Thanks to Richard, for pointing this out.
  NSFileWrapper *wrapper = [NSUnarchiver unarchiveObjectWithData: data]; 

  RELEASE(self);
  return RETAIN(wrapper);
}

- (void) dealloc
{
  TEST_RELEASE(_filename);
  TEST_RELEASE(_fileAttributes);
  TEST_RELEASE(_preferredFilename);
  TEST_RELEASE(_wrapperData);
  TEST_RELEASE(_iconImage);
  [super dealloc];
}

//
// General methods 
//

// write instance to disk at path; if directory type, this
// method is recursive; if updateFilenamesFlag is YES, the wrapper
// will be updated with the name used in writing the file

- (BOOL) writeToFile: (NSString*)path
	  atomically: (BOOL)atomicFlag
     updateFilenames: (BOOL)updateFilenamesFlag
{
  NSFileManager *fm = [NSFileManager defaultManager];
  BOOL success = NO;

  NSDebugLLog(@"NSFileWrapper",
              @"writeToFile: %@ atomically: updateFilenames: ", path);

  switch (_wrapperType)
    {
      case GSFileWrapperDirectoryType: 
        {
          // FIXME - more robust save proceedure when atomicFlag set
          NSEnumerator *enumerator = [_wrapperData keyEnumerator];
          NSString *key;

          [fm createDirectoryAtPath: path
        withIntermediateDirectories: YES
                         attributes: _fileAttributes
                              error: NULL];
          while ((key = (NSString*)[enumerator nextObject]))
            {
              NSString *newPath =
                  [path stringByAppendingPathComponent: key];
              [[_wrapperData objectForKey: key] writeToFile: newPath
                                                 atomically: atomicFlag
                                         updateFilenames: updateFilenamesFlag];
            }
          success = YES;
	  break;
        }
      case GSFileWrapperRegularFileType: 
        {
	  if ([_wrapperData writeToFile: path atomically: atomicFlag])
	    success = [fm changeFileAttributes: _fileAttributes
			  atPath: path];
	  break;
        }
      case GSFileWrapperSymbolicLinkType: 
        {
          success = [fm createSymbolicLinkAtPath: path pathContent: _wrapperData];
	  break;
        }
    }
  if (success && updateFilenamesFlag)
    {
      [self setFilename: [path lastPathComponent]];
    }
  
  return success;
}

- (NSData*) serializedRepresentation
{
  // FIXME - This should use a serializer. To get that working a helper object 
  // is needed that implements the NSObjCTypeSerializationCallBack protocol.
  // We should add this later, currently the NSArchiver is used.
  // Thanks to Richard, for pointing this out.
  return [NSArchiver archivedDataWithRootObject: self]; 
}

- (void) setFilename: (NSString*)filename
{
  if (filename == nil || [filename isEqualToString: @""])
    {
      [NSException raise: NSInternalInconsistencyException
		   format: @"Empty or nil argument to setFilename: "];
    }
  else
    {
      ASSIGN(_filename, filename);
    }
}

- (NSString*) filename
{
  return _filename;
}

- (void) setPreferredFilename: (NSString*)filename
{
  if (filename == nil || [filename isEqualToString: @""])
    {
      [NSException raise: NSInternalInconsistencyException
		   format: @"Empty or nil argument to setPreferredFilename: "];
    }
  else
    {
      ASSIGN(_preferredFilename, filename);
    }
}

- (NSString*) preferredFilename
{
  return _preferredFilename;
}

- (void) setFileAttributes: (NSDictionary*)attributes
{
  if (_fileAttributes == nil)
    {
      _fileAttributes = [NSMutableDictionary new];
    }
  
  [_fileAttributes addEntriesFromDictionary: attributes];
}

- (NSDictionary*) fileAttributes
{
  return _fileAttributes;
}

- (BOOL) isRegularFile
{
  if (_wrapperType == GSFileWrapperRegularFileType)
    {
      return YES;
    }
  else
    { 
      return NO;
    }
}

- (BOOL) isDirectory
{
  if (_wrapperType == GSFileWrapperDirectoryType)
    {
      return YES;
    }
  else
    {
      return NO;
    }
}

- (BOOL) isSymbolicLink
{
  if (_wrapperType == GSFileWrapperSymbolicLinkType)
    {
      return YES;
    }
  else
    {
      return NO;
    }
}

- (void) setIcon: (NSImage*)icon
{
  ASSIGN(_iconImage, icon);
}

- (NSImage*) icon
{
  if (_iconImage == nil && [self filename])
    {
      return [[NSWorkspace sharedWorkspace] iconForFile: [self filename]];
    }
  else
    {
      return _iconImage;
    }
}

- (BOOL) needsToBeUpdatedFromPath: (NSString*)path
{
  NSFileManager *fm = [NSFileManager defaultManager];

  switch (_wrapperType)
    {
      case GSFileWrapperRegularFileType: 
        if ([[self fileAttributes]
                isEqualToDictionary: [fm fileAttributesAtPath: path
                                                 traverseLink: NO]])
          return NO;
        break;
      case GSFileWrapperSymbolicLinkType: 
        if ([_wrapperData isEqualToString: 
                          [fm pathContentOfSymbolicLinkAtPath: path]])
          return NO;
        break;
      case GSFileWrapperDirectoryType: 
	// Has the dictory itself changed?
        if (![[self fileAttributes]
                isEqualToDictionary: [fm fileAttributesAtPath: path
                                                 traverseLink: NO]])
          return YES;

// FIXME - for directory wrappers, we have to check if all the files are still there, 
// if they have the same attributes and if any new files have been added. 
// And this recursive for all included file wrappers

	return NO;
        break;
    }

  return YES;
}

- (BOOL) updateFromPath: (NSString*)path
{
  NSFileManager *fm = [NSFileManager defaultManager];

  switch (_wrapperType)
    {
      case GSFileWrapperRegularFileType: 
        if ([[self fileAttributes]
                isEqualToDictionary: [fm fileAttributesAtPath: path
					 traverseLink: NO]])
          return NO;
	[self initWithPath: path];
        break;
      case GSFileWrapperSymbolicLinkType: 
        if ([[self fileAttributes]
                isEqualToDictionary: [fm fileAttributesAtPath: path
					 traverseLink: NO]] &&
	    [_wrapperData isEqualToString: 
			      [fm pathContentOfSymbolicLinkAtPath: path]])
          return NO;
	[self initWithPath: path];
        break;
      case GSFileWrapperDirectoryType: 
	// Has the dictory itself changed?
        if (![[self fileAttributes]
                isEqualToDictionary: [fm fileAttributesAtPath: path
                                                 traverseLink: NO]])
	{
	  // FIXME: This is not effizent
	  [self initWithPath: path];
          return YES;
	}
// FIXME - for directory wrappers, we have to check if all the files are still there, 
// if they have the same attributes and if any new files have been added. 
// And this recursive for all included file wrappers

	return NO;
        break;
    }

  return YES;
}

//
// Directory type methods 
//

#define GSFileWrapperDirectoryTypeCheck() \
  if (_wrapperType != GSFileWrapperDirectoryType) \
	[NSException raise: NSInternalInconsistencyException \
	            format: @"Can't invoke %@ on a file wrapper that" \
                            @" does not wrap a directory!", _cmd];

- (NSString*) addFileWrapper: (NSFileWrapper*)doc			
{
  NSString *key;

  GSFileWrapperDirectoryTypeCheck();

  key = [doc preferredFilename];
  if (key == nil || [key isEqualToString: @""])
    {
      [NSException raise: NSInvalidArgumentException
                  format: @"Adding file wrapper with no preferred filename."];
      return nil;
    }

  if ([_wrapperData objectForKey: key] != nil)
    {
      // FIXME - handle duplicate names
    }
  [_wrapperData setObject: doc forKey: key];

  return key;
}

- (void) removeFileWrapper: (NSFileWrapper*)doc				
{
  GSFileWrapperDirectoryTypeCheck();

  [_wrapperData removeObjectsForKeys: [_wrapperData allKeysForObject: doc]];
}

- (NSDictionary*) fileWrappers
{
  GSFileWrapperDirectoryTypeCheck();

  return _wrapperData;
}

- (NSString*) keyForFileWrapper: (NSFileWrapper*)doc
{
  GSFileWrapperDirectoryTypeCheck();

  return [[_wrapperData allKeysForObject: doc] objectAtIndex: 0];
}

- (NSString*) addFileWithPath: (NSString*)path
{
  NSFileWrapper *wrapper;
  GSFileWrapperDirectoryTypeCheck();

  wrapper = AUTORELEASE([[NSFileWrapper alloc] initWithPath: path]);
  if (wrapper != nil)
    {
      return [self addFileWrapper: wrapper];
    }
  else
    {
      return nil;
    }
}

- (NSString*) addRegularFileWithContents: (NSData*)data 
                       preferredFilename: (NSString*)filename
{
  NSFileWrapper *wrapper;
  GSFileWrapperDirectoryTypeCheck();

  wrapper = AUTORELEASE([[NSFileWrapper alloc] 
			    initRegularFileWithContents: data]);
  if (wrapper != nil)
    {
      [wrapper setPreferredFilename: filename];
      return [self addFileWrapper: wrapper];
    }
  else
    {
      return nil;
    }
}

- (NSString*) addSymbolicLinkWithDestination: (NSString*)path 
                           preferredFilename: (NSString*)filename
{
  NSFileWrapper *wrapper;
  GSFileWrapperDirectoryTypeCheck();

  wrapper = AUTORELEASE([[NSFileWrapper alloc] 
			    initSymbolicLinkWithDestination: path]);
  if (wrapper != nil)
    {
      [wrapper setPreferredFilename: filename];
      return [self addFileWrapper: wrapper];
    }
  else
    {
      return nil;
    }
}

//								
// Regular file type methods 				  
//								

- (NSData*) regularFileContents
{
  if (_wrapperType == GSFileWrapperRegularFileType)
    {
      return _wrapperData;
    }
  else
    {
      [NSException raise: NSInternalInconsistencyException
		   format: @"File wrapper does not wrap regular file."];
    }
  
  return nil; 
}

//								
// Symbolic link type methods 				  
//

- (NSString*) symbolicLinkDestination
{
  if (_wrapperType == GSFileWrapperSymbolicLinkType)
    {
      return _wrapperData;
    }
  else
    {
      [NSException raise: NSInternalInconsistencyException
		   format: @"File wrapper does not wrap symbolic link."];
    }
  
  return nil;
}

//								
// Archiving 				  
//

- (void) encodeWithCoder: (NSCoder*)aCoder
{
  if ([aCoder allowsKeyedCoding])
    {
      [aCoder encodeObject: [self serializedRepresentation] forKey: @"NSFileWrapperData"];
    }
  else
    {
      [aCoder encodeValueOfObjCType: @encode(int) at: &_wrapperType];
      // Dont store the file name
      [aCoder encodeObject: _preferredFilename];
      [aCoder encodeObject: _fileAttributes];
      [aCoder encodeObject: _wrapperData];
      [aCoder encodeObject: _iconImage];
    }
}

- (id) initWithCoder: (NSCoder*)aDecoder
{
  if ([aDecoder allowsKeyedCoding])
    {
       NSData *data = [aDecoder decodeObjectForKey: @"NSFileWrapperData"];
       return [self initWithSerializedRepresentation: data];
    }
  else
    {
      int wrapperType;
      NSString *preferredFilename;
      NSDictionary *fileAttributes;
      id wrapperData;
      NSImage *iconImage;
      
      [aDecoder decodeValueOfObjCType: @encode(int) at: &wrapperType];
      // Dont restore the file name
      preferredFilename = [aDecoder decodeObject];
      fileAttributes = [aDecoder decodeObject];
      wrapperData = [aDecoder decodeObject];
      iconImage = [aDecoder decodeObject];
      
      switch (wrapperType)
        {
        case GSFileWrapperRegularFileType: 
          {
            self = [self initRegularFileWithContents: wrapperData];
            break;
          }
        case GSFileWrapperSymbolicLinkType: 
          {
            self = [self initSymbolicLinkWithDestination: wrapperData];
            break;
          }
        case GSFileWrapperDirectoryType: 
          {
            self = [self initDirectoryWithFileWrappers: wrapperData];
            break;
          }
        }
      
      if (preferredFilename != nil)
        {
          [self setPreferredFilename: preferredFilename];
        }
      if (fileAttributes != nil)
        {
          [self setFileAttributes: fileAttributes];
        }
      if (iconImage != nil)
        {
          [self setIcon: iconImage];
        }
    }
  return self;
}

@end

