/* 
   AppKit.h

   Main include file for GNUstep GUI Library

   Copyright (C) 1996 Free Software Foundation, Inc.

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

#ifndef _GNUstep_H_AppKit
#define _GNUstep_H_AppKit
#import <GNUstepBase/GSVersionMacros.h>

/* Define library version */
#import <GNUstepGUI/GSVersion.h>

//
// Foundation
//
#import <Foundation/Foundation.h>

//
// GNUstep GUI Library functions
//
#import <AppKit/NSGraphics.h>

#import <AppKit/NSActionCell.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSAttributedString.h>
#import <AppKit/NSBitmapImageRep.h>
#import <AppKit/NSBox.h>
#import <AppKit/NSBrowser.h>
#import <AppKit/NSBrowserCell.h>
#import <AppKit/NSButton.h>
#import <AppKit/NSButtonCell.h>
#import <AppKit/NSCachedImageRep.h>
#import <AppKit/NSCell.h>
#import <AppKit/NSClipView.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSColorList.h>
#import <AppKit/NSColorPanel.h>
#import <AppKit/NSColorPicker.h>
#import <AppKit/NSColorPicking.h>
#import <AppKit/NSColorWell.h>
#import <AppKit/NSControl.h>
#import <AppKit/NSCursor.h>
#import <AppKit/NSCustomImageRep.h>
#import <AppKit/NSDataLink.h>
#import <AppKit/NSDataLinkManager.h>
#import <AppKit/NSDataLinkPanel.h>
#import <AppKit/NSDragging.h>
#import <AppKit/NSEPSImageRep.h>
#import <AppKit/NSEvent.h>
#import <AppKit/NSFont.h>
#import <AppKit/NSFontManager.h>
#import <AppKit/NSFontPanel.h>
#import <AppKit/NSForm.h>
#import <AppKit/NSFormCell.h>
#import <AppKit/NSHelpPanel.h>
#import <AppKit/NSImage.h>
#import <AppKit/NSImageCell.h>
#import <AppKit/NSImageRep.h>
#import <AppKit/NSImageView.h>
#import <AppKit/NSMatrix.h>
#import <AppKit/NSMenu.h>
#import <AppKit/NSMenuItem.h>
#import <AppKit/NSMenuItemCell.h>
#import <AppKit/NSMenuView.h>
#import <AppKit/NSNibLoading.h>
#import <AppKit/NSOpenPanel.h>
#import <AppKit/NSPageLayout.h>
#import <AppKit/NSPanel.h>
#import <AppKit/NSPasteboard.h>
#import <AppKit/NSPopUpButton.h>
#import <AppKit/NSPopUpButtonCell.h>
#import <AppKit/NSPrinter.h>
#import <AppKit/NSPrintInfo.h>
#import <AppKit/NSPrintOperation.h>
#import <AppKit/NSPrintPanel.h>
#import <AppKit/NSResponder.h>
#import <AppKit/NSSavePanel.h>
#import <AppKit/NSScreen.h>
#import <AppKit/NSScroller.h>
#import <AppKit/NSScrollView.h>
#import <AppKit/NSSelection.h>
#import <AppKit/NSSlider.h>
#import <AppKit/NSSliderCell.h>
#import <AppKit/NSSpellChecker.h>
#import <AppKit/NSSpellProtocol.h>
#import <AppKit/NSSplitView.h>
#import <AppKit/NSStringDrawing.h>
#import <AppKit/NSText.h>
#import <AppKit/NSTextField.h>
#import <AppKit/NSTextFieldCell.h>
#import <AppKit/NSView.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSWorkspace.h>

#if OS_API_VERSION(GS_API_MACOSX, GS_API_LATEST)
#import <AppKit/NSAlert.h>
#import <AppKit/NSAnimation.h>
#import <AppKit/NSAffineTransform.h>
#import <AppKit/NSArrayController.h>
#import <AppKit/NSBezierPath.h>
#import <AppKit/NSColorSpace.h>
#import <AppKit/NSComboBox.h>
#import <AppKit/NSComboBoxCell.h>
#import <AppKit/NSController.h>
#import <AppKit/NSDocument.h>
#import <AppKit/NSDocumentController.h>
#import <AppKit/NSDrawer.h>
#import <AppKit/NSFileWrapper.h>
#import <AppKit/NSFontDescriptor.h>
#import <AppKit/NSGradient.h>
#import <AppKit/NSGraphicsContext.h>
#import <AppKit/NSHelpManager.h>
#import <AppKit/NSInputManager.h>
#import <AppKit/NSInputServer.h>
#import <AppKit/NSInterfaceStyle.h>
#import <AppKit/NSKeyValueBinding.h>
#import <AppKit/NSLayoutManager.h>
#import <AppKit/NSLevelIndicator.h>
#import <AppKit/NSLevelIndicatorCell.h>
#import <AppKit/NSMovie.h>
#import <AppKit/NSMovieView.h>
#import <AppKit/NSNib.h>
#import <AppKit/NSNibDeclarations.h>
#import <AppKit/NSObjectController.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#import <AppKit/NSOutlineView.h>
#import <AppKit/NSParagraphStyle.h>
#import <AppKit/NSProgressIndicator.h>
#import <AppKit/NSRulerMarker.h>
#import <AppKit/NSRulerView.h>
#import <AppKit/NSSearchField.h>
#import <AppKit/NSSearchFieldCell.h>
#import <AppKit/NSSecureTextField.h>
#import <AppKit/NSSegmentedCell.h>
#import <AppKit/NSSegmentedControl.h>
#import <AppKit/NSShadow.h>
#import <AppKit/NSSound.h>
#import <AppKit/NSSpeechSynthesizer.h>
#import <AppKit/NSStepper.h>
#import <AppKit/NSStepperCell.h>
#import <AppKit/NSTableColumn.h>
#import <AppKit/NSTableHeaderCell.h>
#import <AppKit/NSTableHeaderView.h>
#import <AppKit/NSTableView.h>
#import <AppKit/NSTabView.h>
#import <AppKit/NSTabViewItem.h>
#import <AppKit/NSTextAttachment.h>
#import <AppKit/NSTextContainer.h>
#import <AppKit/NSTextList.h>
#import <AppKit/NSTextStorage.h>
#import <AppKit/NSTextTable.h>
#import <AppKit/NSTextView.h>
#import <AppKit/NSToolbar.h>
#import <AppKit/NSToolbarItem.h>
#import <AppKit/NSUserDefaultsController.h>
#import <AppKit/NSUserInterfaceValidation.h>
#import <AppKit/NSViewController.h>
#import <AppKit/NSWindowController.h>
#endif

#import <AppKit/PSOperators.h>

#endif /* _GNUstep_H_AppKit */
