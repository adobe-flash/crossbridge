/* 
   NSTreeController.h

   Main include file for GNUstep GUI Library

   Copyright (C) 2012 Free Software Foundation, Inc.

   Author:  Gregory Casamento <greg.casamento@gmail.com>
   Date: 2012
   
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

#import <AppKit/NSController.h>

#ifndef _GNUstep_H_NSTreeController
#define _GNUstep_H_NSTreeController

@interface NSTreeController : NSController <NSCoding, NSCopying>
{
}

#if 0 // compile this out for now....
- (BOOL) addSelectionIndexPaths:(NSArray *)indexPaths;
- (BOOL) alwaysUsesMultipleValuesMarker;
- (BOOL) avoidsEmptySelection;
- (BOOL) canAddChid;
- (BOOL) canInsert;
- (BOOL) canInsertChild;
- (BOOL) preservesSelection; 
- (BOOL) selectsInsertedObjects;
- (BOOL) setSelectionIndexPath: (NSIndexPath *)indexPath;
- (BOOL) setSelectionIndexPaths: (NSArray *)indexPaths;
- (id) arrangedObjects;
- (id) content;
- (NSArray *) selectedObjects;
- (NSArray *) selectionIndexPaths;
- (NSArray *) sortDescriptors;
- (NSIndexPath *) selectionIndexPath;
- (NSString *) childrenKeyPath;
- (NSString *) countKeyPath;
- (NSString *) leafKeyPath;
- (void) addChild: (id)sender;
- (void) add: (id)sender;
- (void) insertChild: (id)sender;
- (void) insertObject: (id)object atArrangedObjectIndexPath:(NSIndexPath *)indexPath;
- (void) insertObjects: (NSArray *)objects atArrangedObjectIndexPaths: (NSArray *)indexPaths;
- (void) insert:sender;
- (void) rearrangeObjects;
- (void) removeObjectAtArrangedObjectIndexPath: (NSIndexPath *)indexPath;
- (void) removeObjectsAtArrangedObjectIndexPaths: (NSArray *)indexPaths;
- (void) removeSelectionIndexPaths: (NSArray *)indexPaths;
- (void) remove: (id)sender;
- (void) setAlwaysUsesMultipleValuesMarker:(BOOL)flag;
- (void) setAvoidsEmptySelection:(BOOL)flag;
- (void) setChildrenKeyPath:(NSString *)path;
- (void) setContent: (id)content;
- (void) setCountKeyPath: (NSString *)path;
- (void) setLeafPathKey: (NSString *)key;
- (void) setPreservesSelection: (BOOL)flag;
- (void) setSelectsInsertedObjects: (BOOL)flag;
- (void) setSortDescriptors: (NSArray *)descriptors;
#endif

@end

#endif /* _GNUstep_H_NSTreeController */
