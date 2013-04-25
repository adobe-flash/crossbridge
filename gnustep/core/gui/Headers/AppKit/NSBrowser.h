/* 
   NSBrowser.h

   Control to display and select from hierarchal lists

   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

   Author:  Scott Christley <scottc@net-community.com>
   Date: 1996
   
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

#ifndef _GNUstep_H_NSBrowser
#define _GNUstep_H_NSBrowser
#import <GNUstepBase/GSVersionMacros.h>

#import <AppKit/NSControl.h>

@class NSString;
@class NSArray;

@class NSCell;
@class NSMatrix;
@class NSScroller;
//@class NSBox;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
typedef enum _NSBrowserColumnResizingType
{
  NSBrowserNoColumnResizing,
  NSBrowserAutoColumnResizing,
  NSBrowserUserColumnResizing
} NSBrowserColumnResizingType;
#endif

@interface NSBrowser : NSControl <NSCoding>
{
  // Attributes
  NSCell *_browserCellPrototype;
  Class _browserMatrixClass;
  NSString *_pathSeparator;
  
  //NSBox *_horizontalScrollerBox;
  NSScroller *_horizontalScroller;
  NSTimeInterval _lastKeyPressed;
  NSString *_charBuffer;

  BOOL _isLoaded;
  BOOL _allowsBranchSelection;
  BOOL _allowsEmptySelection;
  BOOL _allowsMultipleSelection;
  BOOL _reusesColumns;
  BOOL _separatesColumns;
  BOOL _takesTitleFromPreviousColumn;
  BOOL _isTitled;
  BOOL _hasHorizontalScroller;
  BOOL _skipUpdateScroller;
  BOOL _acceptsArrowKeys;
  BOOL _sendsActionOnArrowKeys;
  BOOL _acceptsAlphaNumericalKeys;
  BOOL _sendsActionOnAlphaNumericalKeys;
  BOOL _prefersAllColumnUserResizing;

  BOOL _passiveDelegate;
  id _browserDelegate;
  id _target;
  SEL _action;
  SEL _doubleAction;
  NSMutableArray *_browserColumns;
  NSSize _columnSize;
  NSRect _scrollerRect;
  int _alphaNumericalLastColumn;
  int _maxVisibleColumns;
  float _minColumnWidth;
  int _lastColumnLoaded;
  int _firstVisibleColumn;
  int _lastVisibleColumn;
  NSString *_columnsAutosaveName;
	NSBrowserColumnResizingType _columnResizing;
}

//
// Setting the Delegate 
//
- (id) delegate;
- (void) setDelegate: (id)anObject;

//
// Target and Action 
//
- (SEL) doubleAction;
- (BOOL) sendAction;
- (void) setDoubleAction: (SEL)aSelector;

//
// Setting Component Classes 
//
+ (Class) cellClass;
- (id) cellPrototype;
- (Class) matrixClass;
- (void) setCellClass: (Class)classId;
- (void) setCellPrototype: (NSCell *)aCell;
- (void) setMatrixClass: (Class)classId;

//
// Setting NSBrowser Behavior 
//
- (BOOL) reusesColumns;
- (void) setReusesColumns: (BOOL)flag;
- (void) setTakesTitleFromPreviousColumn: (BOOL)flag;
- (BOOL) takesTitleFromPreviousColumn;

//
// Allowing Different Types of Selection 
//
- (BOOL) allowsBranchSelection;
- (BOOL) allowsEmptySelection;
- (BOOL) allowsMultipleSelection;
- (void) setAllowsBranchSelection: (BOOL)flag;
- (void) setAllowsEmptySelection: (BOOL)flag;
- (void) setAllowsMultipleSelection: (BOOL)flag;

//
// Setting Arrow Key Behavior
//
- (BOOL) acceptsArrowKeys;
- (BOOL) sendsActionOnArrowKeys;
- (void) setAcceptsArrowKeys: (BOOL)flag;
- (void) setSendsActionOnArrowKeys: (BOOL)flag;

//
// Showing a Horizontal Scroller 
//
- (void) setHasHorizontalScroller: (BOOL)flag;
- (BOOL) hasHorizontalScroller;

//
// Setting the NSBrowser's Appearance 
//
- (int) maxVisibleColumns;
- (int) minColumnWidth;
- (BOOL) separatesColumns;
- (void) setMaxVisibleColumns: (int)columnCount;
- (void) setMinColumnWidth: (int)columnWidth;
- (void) setSeparatesColumns: (BOOL)flag;
#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
- (float) columnWidthForColumnContentWidth: (float)columnContentWidth;
- (float) columnContentWidthForColumnWidth: (float)columnWidth;
#endif

//
// Manipulating Columns 
//
- (void) addColumn;
- (int) columnOfMatrix: (NSMatrix *)matrix;
- (void) displayAllColumns;
- (void) displayColumn: (int)column;
- (int) firstVisibleColumn;
- (BOOL) isLoaded;
- (int) lastColumn;
- (int) lastVisibleColumn;
- (void) loadColumnZero;
- (int) numberOfVisibleColumns;
- (void) reloadColumn: (int)column;
- (void) selectAll: (id)sender;
- (void) selectRow: (int)row inColumn: (int)column;
- (int) selectedColumn;
- (int) selectedRowInColumn: (int)column;
- (void) setLastColumn: (int)column;
- (void) validateVisibleColumns;

//
// Manipulating Column Titles 
//
- (void) drawTitle: (NSString *)title
	    inRect: (NSRect)aRect
	  ofColumn: (int)column;
#if OS_API_VERSION(GS_API_MACOSX, GS_API_LATEST)
- (void) drawTitleOfColumn: (int)column 
		    inRect: (NSRect)aRect;
#endif 
- (BOOL) isTitled;
- (void) setTitled: (BOOL)flag;
- (void) setTitle: (NSString *)aString
	 ofColumn: (int)column;
- (NSRect) titleFrameOfColumn: (int)column;
- (float) titleHeight;
- (NSString *) titleOfColumn: (int)column;

//
// Scrolling an NSBrowser 
//
- (void) scrollColumnsLeftBy: (int)shiftAmount;
- (void) scrollColumnsRightBy: (int)shiftAmount;
- (void) scrollColumnToVisible: (int)column;
- (void) scrollViaScroller: (NSScroller *)sender;
- (void) updateScroller;
#if OS_API_VERSION(MAC_OS_X_VERSION_10_6, GS_API_LATEST)
- (void) scrollRowToVisible: (NSInteger)row inColumn: (NSInteger)column;
#endif

//
// Event Handling 
//
- (void) doClick: (id)sender;
- (void) doDoubleClick: (id)sender;

//
// Getting Matrices and Cells 
//
- (id) loadedCellAtRow: (int)row
	        column: (int)column;
- (NSMatrix *) matrixInColumn: (int)column;
- (id) selectedCell;
- (id) selectedCellInColumn: (int)column;
- (NSArray *) selectedCells;

//
// Getting Column Frames 
//
- (NSRect) frameOfColumn: (int)column;
- (NSRect) frameOfInsideOfColumn: (int)column;

//
// Manipulating Paths 
//
- (NSString *) path;
- (NSString *) pathSeparator;
- (NSString *) pathToColumn: (int)column;
- (BOOL) setPath: (NSString *)path;
- (void) setPathSeparator: (NSString *)aString;

//
// Arranging an NSBrowser's Components 
//
- (void) tile;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
//
// Resizing
//
- (NSBrowserColumnResizingType) columnResizingType;
- (void) setColumnResizingType:(NSBrowserColumnResizingType) type;
- (BOOL) prefersAllColumnUserResizing;
- (void) setPrefersAllColumnUserResizing: (BOOL)flag;
- (float) widthOfColumn: (int)column;
- (void) setWidth: (float)columnWidth ofColumn: (int)columnIndex;

//
// Autosave names
//
+ (void) removeSavedColumnsWithAutosaveName: (NSString *)name;
- (NSString *) columnsAutosaveName;
- (void) setColumnsAutosaveName: (NSString *)name;
#endif
@end

//
// Controlling the alphanumerical keys behaviour
//
@interface NSBrowser (GNUstepExtensions)
- (BOOL)acceptsAlphaNumericalKeys;
- (void) setAcceptsAlphaNumericalKeys: (BOOL)flag;
- (BOOL) sendsActionOnAlphaNumericalKeys;
- (void) setSendsActionOnAlphaNumericalKeys: (BOOL)flag;
@end

//
// Methods Implemented by the Delegate 
//
@interface NSObject (NSBrowserDelegate)

- (void) browser: (NSBrowser *)sender createRowsForColumn: (int)column
  inMatrix: (NSMatrix *)matrix;
/** Returns YES iff */
- (BOOL) browser: (NSBrowser *)sender isColumnValid: (int)column;
- (int) browser: (NSBrowser *)sender numberOfRowsInColumn: (int)column;
- (BOOL) browser: (NSBrowser *)sender selectCellWithString: (NSString *)title
  inColumn: (int)column;
- (BOOL) browser: (NSBrowser *)sender selectRow: (int)row inColumn: (int)column;
- (NSString *) browser: (NSBrowser *)sender titleOfColumn: (int)column;
- (void) browser: (NSBrowser *)sender
 willDisplayCell: (id)cell
           atRow: (int)row
          column: (int)column;
- (void) browserDidScroll: (NSBrowser *)sender;
- (void) browserWillScroll: (NSBrowser *)sender;

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
- (float) browser: (NSBrowser *)browser
 shouldSizeColumn: (int)column
    forUserResize: (BOOL)flag
          toWidth: (float)width;
- (float) browser: (NSBrowser *)browser
sizeToFitWidthOfColumn: (int)column;
- (void) browserColumnConfigurationDidChange: (NSNotification *)notification;
#endif
@end

#if OS_API_VERSION(MAC_OS_X_VERSION_10_3, GS_API_LATEST)
APPKIT_EXPORT NSString *NSBrowserColumnConfigurationDidChangeNotification;
#endif

#endif // _GNUstep_H_NSBrowser
