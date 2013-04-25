/*
   externs.m

   External data

   Copyright (C) 1997 Free Software Foundation, Inc.

   Author:  Scott Christley <scottc@net-community.com>
   Date: August 1997

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
#import <Foundation/NSString.h>
#import "AppKit/NSEvent.h"

// Global strings
NSString *NSModalPanelRunLoopMode = @"ModalPanelMode";
NSString *NSEventTrackingRunLoopMode = @"EventTrackingMode";

//
// Global Exception Strings
//
NSString *NSAbortModalException = @"AbortModal";
NSString *NSAbortPrintingException = @"AbortPrinting";
NSString *NSAppKitIgnoredException = @"AppKitIgnored";
NSString *NSAppKitVirtualMemoryException = @"AppKitVirtualMemory";
NSString *NSBadBitmapParametersException = @"BadBitmapParameters";
NSString *NSBadComparisonException = @"BadComparison";
NSString *NSBadRTFColorTableException = @"BadRTFColorTable";
NSString *NSBadRTFDirectiveException = @"BadRTFDirective";
NSString *NSBadRTFFontTableException = @"BadRTFFontTable";
NSString *NSBadRTFStyleSheetException = @"BadRTFStyleSheet";
NSString *NSBrowserIllegalDelegateException = @"BrowserIllegalDelegate";
NSString *NSColorListIOException = @"ColorListIO";
NSString *NSColorListNotEditableException = @"ColorListNotEditable";
NSString *NSDraggingException = @"Draggin";
NSString *NSFontUnavailableException = @"FontUnavailable";
NSString *NSIllegalSelectorException = @"IllegalSelector";
NSString *NSImageCacheException = @"ImageCache";
NSString *NSNibLoadingException = @"NibLoading";
NSString *NSPPDIncludeNotFoundException = @"PPDIncludeNotFound";
NSString *NSPPDIncludeStackOverflowException = @"PPDIncludeStackOverflow";
NSString *NSPPDIncludeStackUnderflowException = @"PPDIncludeStackUnderflow";
NSString *NSPPDParseException = @"PPDParse";
NSString *NSPrintOperationExistsException = @"PrintOperationExists";
NSString *NSPrintPackageException = @"PrintPackage";
NSString *NSPrintingCommunicationException = @"PrintingCommunication";
NSString *NSRTFPropertyStackOverflowException = @"RTFPropertyStackOverflow";
NSString *NSTIFFException = @"TIFF";
NSString *NSTextLineTooLongException = @"TextLineTooLong";
NSString *NSTextNoSelectionException = @"TextNoSelection";
NSString *NSTextReadException = @"TextRead";
NSString *NSTextWriteException = @"TextWrite";
NSString *NSTypedStreamVersionException = @"TypedStreamVersion";
NSString *NSWindowServerCommunicationException = @"WindowServerCommunication";
NSString *NSWordTablesReadException = @"WordTablesRead";
NSString *NSWordTablesWriteException = @"WordTablesWrite";

NSString *GSWindowServerInternalException = @"WindowServerInternal";

// NSAnimation
NSString* NSAnimationProgressMarkNotification
= @"NSAnimationProgressMarkNotification";
NSString *NSAnimationProgressMark = @"NSAnimationProgressMark";
NSString *NSAnimationTriggerOrderIn = @"NSAnimationTriggerOrderIn"; 
NSString *NSAnimationTriggerOrderOut = @"NSAnimationTriggerOrderOut"; 

// Application notifications
NSString *NSApplicationDidBecomeActiveNotification
              = @"ApplicationDidBecomeActive";
NSString *NSApplicationDidChangeScreenParametersNotification 
              = @"NSApplicationDidChangeScreenParameters";
NSString *NSApplicationDidFinishLaunchingNotification
              = @"ApplicationDidFinishLaunching";
NSString *NSApplicationDidHideNotification = @"ApplicationDidHide";
NSString *NSApplicationDidResignActiveNotification
              = @"ApplicationDidResignActive";
NSString *NSApplicationDidUnhideNotification = @"ApplicationDidUnhide";
NSString *NSApplicationDidUpdateNotification = @"ApplicationDidUpdate";
NSString *NSApplicationWillBecomeActiveNotification
              = @"ApplicationWillBecomeActive";
NSString *NSApplicationWillFinishLaunchingNotification
              = @"ApplicationWillFinishLaunching";
NSString *NSApplicationWillTerminateNotification = @"ApplicationWillTerminate";
NSString *NSApplicationWillHideNotification = @"ApplicationWillHide";
NSString *NSApplicationWillResignActiveNotification
              = @"ApplicationWillResignActive";
NSString *NSApplicationWillUnhideNotification = @"ApplicationWillUnhide";
NSString *NSApplicationWillUpdateNotification = @"ApplicationWillUpdate";

// NSBitmapImageRep Global strings
NSString *NSImageCompressionMethod = @"NSImageCompressionMethod";
NSString *NSImageCompressionFactor = @"NSImageCompressionFactor";
NSString *NSImageDitherTransparency = @"NSImageDitherTransparency";
NSString *NSImageRGBColorTable = @"NSImageRGBColorTable";
NSString *NSImageInterlaced = @"NSImageInterlaced";
NSString *NSImageColorSyncProfileData = @"NSImageColorSyncProfileData";  // Mac OS X only
//NSString *GSImageICCProfileData = @"GSImageICCProfileData";  // if & when GNUstep supports color management
NSString *NSImageFrameCount = @"NSImageFrameCount";
NSString *NSImageCurrentFrame = @"NSImageCurrentFrame";
NSString *NSImageCurrentFrameDuration = @"NSImageCurrentFrameDuration";
NSString *NSImageLoopCount = @"NSImageLoopCount";
NSString *NSImageGamma = @"NSImageGamma";
NSString *NSImageProgressive = @"NSImageProgressive";
NSString *NSImageEXIFData = @"NSImageEXIFData";  // No support yet in GNUstep

// NSBrowser notification
NSString *NSBrowserColumnConfigurationDidChangeNotification = @"NSBrowserColumnConfigurationDidChange";

// NSColor Global strings
NSString *NSCalibratedWhiteColorSpace = @"NSCalibratedWhiteColorSpace";
NSString *NSCalibratedBlackColorSpace = @"NSCalibratedBlackColorSpace";
NSString *NSCalibratedRGBColorSpace = @"NSCalibratedRGBColorSpace";
NSString *NSDeviceWhiteColorSpace = @"NSDeviceWhiteColorSpace";
NSString *NSDeviceBlackColorSpace = @"NSDeviceBlackColorSpace";
NSString *NSDeviceRGBColorSpace = @"NSDeviceRGBColorSpace";
NSString *NSDeviceCMYKColorSpace = @"NSDeviceCMYKColorSpace";
NSString *NSNamedColorSpace = @"NSNamedColorSpace";
NSString *NSPatternColorSpace = @"NSPatternColorSpace";
NSString *NSCustomColorSpace = @"NSCustomColorSpace";

// NSColor Global gray values
const float NSBlack = 0;
const float NSDarkGray = .333;
const float NSGray = 0.5;
const float NSLightGray = .667;
const float NSWhite = 1;

// NSColor notification
NSString *NSSystemColorsDidChangeNotification =
            @"NSSystemColorsDidChangeNotification";

// NSColorList notifications
NSString *NSColorListDidChangeNotification = @"NSColorListDidChange";

// NSColorPanel notifications
NSString *NSColorPanelColorDidChangeNotification =
  @"NSColorPanelColorDidChange";

#if GNUSTEP_GUI_MAJOR_VERSION == 0 && GNUSTEP_GUI_MINOR_VERSION < 20
/* The above notifications had been misspelled in GNUstep. We keep them around
   here to preserve binary compatibility until the next release. */
NSString *NSColorListChangedNotification = @"NSColorListDidChange";
NSString *NSColorPanelColorChangedNotification = @"NSColorPanelColorDidChange";
#endif

// NSComboBox notifications
NSString *NSComboBoxWillPopUpNotification = 
@"NSComboBoxWillPopUpNotification";
NSString *NSComboBoxWillDismissNotification = 
@"NSComboBoxWillDismissNotification";
NSString *NSComboBoxSelectionDidChangeNotification = 
@"NSComboBoxSelectionDidChangeNotification";
NSString *NSComboBoxSelectionIsChangingNotification = 
@"NSComboBoxSelectionIsChangingNotification";

// NSControl notifications
NSString *NSControlTextDidBeginEditingNotification =
@"NSControlTextDidBeginEditingNotification";
NSString *NSControlTextDidEndEditingNotification =
@"NSControlTextDidEndEditingNotification";
NSString *NSControlTextDidChangeNotification =
@"NSControlTextDidChangeNotification";

// NSDataLink global strings
NSString *NSDataLinkFilenameExtension = @"dlf";

// NSDrawer notifications
NSString *NSDrawerDidCloseNotification =
@"NSDrawerDidCloseNotification";
NSString *NSDrawerDidOpenNotification =
@"NSDrawerDidOpenNotification";
NSString *NSDrawerWillCloseNotification =
@"NSDrawerWillCloseNotification";
NSString *NSDrawerWillOpenNotification =
@"NSDrawerWillOpenNotification";

// NSForm private notification
NSString *_NSFormCellDidChangeTitleWidthNotification 
= @"_NSFormCellDidChangeTitleWidthNotification";

// NSGraphicContext constants
NSString *NSGraphicsContextDestinationAttributeName = 
@"NSGraphicsContextDestinationAttributeName";
NSString *NSGraphicsContextPDFFormat = 
@"NSGraphicsContextPDFFormat";
NSString *NSGraphicsContextPSFormat = 
@"NSGraphicsContextPSFormat";
NSString *NSGraphicsContextRepresentationFormatAttributeName = 
@"NSGraphicsContextRepresentationFormatAttributeName";

NSString *NSImageInterpolationDefault = @"NSImageInterpolationDefault";
NSString *NSImageInterpolationNone = @"NSImageInterpolationNone";
NSString *NSImageInterpolationLow = @"NSImageInterpolationLow";
NSString *NSImageInterpolationHigh = @"NSImageInterpolationHigh";

// NSHelpManager notifications;
NSString *NSContextHelpModeDidActivateNotification =
@"NSContextHelpModeDidActivateNotification";
NSString *NSContextHelpModeDidDeactivateNotification =
@"NSContextHelpModeDidDeactivateNotification";

// NSFont Global Strings
NSString *NSAFMAscender = @"Ascender";
NSString *NSAFMCapHeight = @"CapHeight";
NSString *NSAFMCharacterSet = @"CharacterSet";
NSString *NSAFMDescender = @"Descender";
NSString *NSAFMEncodingScheme = @"EncodingScheme";
NSString *NSAFMFamilyName = @"FamilyName";
NSString *NSAFMFontName = @"FontName";
NSString *NSAFMFormatVersion = @"FormatVersion";
NSString *NSAFMFullName = @"FullName";
NSString *NSAFMItalicAngle = @"ItalicAngle";
NSString *NSAFMMappingScheme = @"MappingScheme";
NSString *NSAFMNotice = @"Notice";
NSString *NSAFMUnderlinePosition = @"UnderlinePosition";
NSString *NSAFMUnderlineThickness = @"UnderlineThickness";
NSString *NSAFMVersion = @"Version";
NSString *NSAFMWeight = @"Weight";
NSString *NSAFMXHeight = @"XHeight";

// NSFontDescriptor global strings
NSString *NSFontFamilyAttribute = @"Family";
NSString *NSFontNameAttribute = @"Name";
NSString *NSFontFaceAttribute = @"Face";
NSString *NSFontSizeAttribute = @"Size"; 
NSString *NSFontVisibleNameAttribute = @"VisibleName"; 
NSString *NSFontColorAttribute = @"Color";
NSString *NSFontMatrixAttribute = @"Matrix";
NSString *NSFontVariationAttribute = @"Variation";
NSString *NSFontCharacterSetAttribute = @"CharacterSet";
NSString *NSFontCascadeListAttribute = @"CascadeList";
NSString *NSFontTraitsAttribute = @"Traits";
NSString *NSFontFixedAdvanceAttribute = @"FixedAdvance";

NSString *NSFontSymbolicTrait = @"SymbolicTrait";
NSString *NSFontWeightTrait = @"WeightTrait";
NSString *NSFontWidthTrait = @"WidthTrait";
NSString *NSFontSlantTrait = @"SlantTrait";

NSString *NSFontVariationAxisIdentifierKey = @"VariationAxisIdentifier";
NSString *NSFontVariationAxisMinimumValueKey = @"VariationAxisMinimumValue";
NSString *NSFontVariationAxisMaximumValueKey = @"VariationAxisMaximumValue";
NSString *NSFontVariationAxisDefaultValueKey = @"VariationAxisDefaultValue";
NSString *NSFontVariationAxisNameKey = @"VariationAxisName";

// NSScreen Global device dictionary key strings
NSString *NSDeviceResolution = @"NSDeviceResolution";
NSString *NSDeviceColorSpaceName = @"NSDeviceColorSpaceName";
NSString *NSDeviceBitsPerSample = @"NSDeviceBitsPerSample";
NSString *NSDeviceIsScreen = @"NSDeviceIsScreen";
NSString *NSDeviceIsPrinter = @"NSDeviceIsPrinter";
NSString *NSDeviceSize = @"NSDeviceSize";

// NSImageRep notifications
NSString *NSImageRepRegistryChangedNotification =
@"NSImageRepRegistryChangedNotification";

// Pasteboard Type Globals
NSString *NSStringPboardType = @"NSStringPboardType";
NSString *NSColorPboardType = @"NSColorPboardType";
NSString *NSFileContentsPboardType = @"NSFileContentsPboardType";
NSString *NSFilenamesPboardType = @"NSFilenamesPboardType";
NSString *NSFontPboardType = @"NSFontPboardType";
NSString *NSRulerPboardType = @"NSRulerPboardType";
NSString *NSPostScriptPboardType = @"NSPostScriptPboardType";
NSString *NSTabularTextPboardType = @"NSTabularTextPboardType";
NSString *NSRTFPboardType = @"NSRTFPboardType";
NSString *NSRTFDPboardType = @"NSRTFDPboardType";
NSString *NSTIFFPboardType = @"NSTIFFPboardType";
NSString *NSDataLinkPboardType = @"NSDataLinkPboardType";
NSString *NSGeneralPboardType = @"NSGeneralPboardType";
NSString *NSPDFPboardType = @"NSPDFPboardType";
NSString *NSPICTPboardType = @"NSPICTPboardType";
NSString *NSURLPboardType = @"NSURLPboardType";
NSString *NSHTMLPboardType = @"NSHTMLPboardType";
NSString *NSVCardPboardType = @"NSVCardPboardType";
NSString *NSFilesPromisePboardType = @"NSFilesPromisePboardType";

// Pasteboard Name Globals
NSString *NSDragPboard = @"NSDragPboard";
NSString *NSFindPboard = @"NSFindPboard";
NSString *NSFontPboard = @"NSFontPboard";
NSString *NSGeneralPboard = @"NSGeneralPboard";
NSString *NSRulerPboard = @"NSRulerPboard";

//
// Pasteboard Exceptions
//
NSString *NSPasteboardCommunicationException
= @"NSPasteboardCommunicationException";

// Printing Information Dictionary Keys
NSString *NSPrintAllPages = @"PrintAllPages";
NSString *NSPrintBottomMargin = @"PrintBottomMargin";
NSString *NSPrintCopies = @"PrintCopies";
NSString *NSPrintFaxCoverSheetName = @"PrintFaxCoverSheetName";
NSString *NSPrintFaxHighResolution = @"PrintFaxHighResolution";
NSString *NSPrintFaxModem = @"PrintFaxModem";
NSString *NSPrintFaxReceiverNames = @"PrintFaxReceiverNames";
NSString *NSPrintFaxReceiverNumbers = @"PrintFaxReceiverNumbers";
NSString *NSPrintFaxReturnReceipt = @"PrintFaxReturnReceipt";
NSString *NSPrintFaxSendTime = @"PrintFaxSendTime";
NSString *NSPrintFaxTrimPageEnds = @"PrintFaxTrimPageEnds";
NSString *NSPrintFaxUseCoverSheet = @"PrintFaxUseCoverSheet";
NSString *NSPrintFirstPage = @"PrintFirstPage";
NSString *NSPrintHorizonalPagination = @"PrintHorizonalPagination";
NSString *NSPrintHorizontallyCentered = @"PrintHorizontallyCentered";
NSString *NSPrintJobDisposition = @"PrintJobDisposition";
NSString *NSPrintJobFeatures = @"PrintJobFeatures";
NSString *NSPrintLastPage = @"PrintLastPage";
NSString *NSPrintLeftMargin = @"PrintLeftMargin";
NSString *NSPrintManualFeed = @"PrintManualFeed";
NSString *NSPrintMustCollate = @"PrintMustCollate";
NSString *NSPrintOrientation = @"PrintOrientation";
NSString *NSPrintPagesPerSheet = @"PrintPagesPerSheet";
NSString *NSPrintPaperFeed = @"PrintPaperFeed";
NSString *NSPrintPaperName = @"PrintPaperName";
NSString *NSPrintPaperSize = @"PrintPaperSize";
NSString *NSPrintPrinter = @"PrintPrinter";
NSString *NSPrintReversePageOrder = @"PrintReversePageOrder";
NSString *NSPrintRightMargin = @"PrintRightMargin";
NSString *NSPrintSavePath = @"PrintSavePath";
NSString *NSPrintScalingFactor = @"PrintScalingFactor";
NSString *NSPrintTopMargin = @"PrintTopMargin";
NSString *NSPrintHorizontalPagination = @"PrintHorizontalPagination";
NSString *NSPrintVerticalPagination = @"PrintVerticalPagination";
NSString *NSPrintVerticallyCentered = @"PrintVerticallyCentered";
NSString *NSPrintPagesAcross = @"PrintPagesAcross";
NSString *NSPrintPagesDown = @"PrintPagesDown";
NSString *NSPrintTime = @"PrintTime";
NSString *NSPrintDetailedErrorReporting = @"PrintDetailedErrorReporting";
NSString *NSPrintFaxNumber = @"PrintFaxNumber";
NSString *NSPrintPrinterName = @"PrintPrinterName";
NSString *NSPrintHeaderAndFooter = @"PrintHeaderAndFooter";

NSString *NSPrintPageDirection = @"NSPrintPageDirection";

// Print Job Disposition Values
NSString  *NSPrintCancelJob = @"PrintCancelJob";
NSString  *NSPrintFaxJob = @"PrintFaxJob";
NSString  *NSPrintPreviewJob = @"PrintPreviewJob";
NSString  *NSPrintSaveJob = @"PrintSaveJob";
NSString  *NSPrintSpoolJob = @"PrintSpoolJob";

// Print Panel
NSString *NSPrintPanelAccessorySummaryItemNameKey = @"name";
NSString *NSPrintPanelAccessorySummaryItemDescriptionKey = @"description";
NSString *NSPrintPhotoJobStyleHint = @"Photo";

// NSSplitView notifications
NSString *NSSplitViewDidResizeSubviewsNotification =
@"NSSplitViewDidResizeSubviewsNotification";
NSString *NSSplitViewWillResizeSubviewsNotification =
@"NSSplitViewWillResizeSubviewsNotification";

// NSTableView notifications
NSString *NSTableViewColumnDidMove = @"NSTableViewColumnDidMoveNotification";
NSString *NSTableViewColumnDidResize 
= @"NSTableViewColumnDidResizeNotification";
NSString *NSTableViewSelectionDidChange 
= @"NSTableViewSelectionDidChangeNotification";
NSString *NSTableViewSelectionIsChanging 
= @"NSTableViewSelectionIsChangingNotification";

// NSText notifications
NSString *NSTextDidBeginEditingNotification =
@"NSTextDidBeginEditingNotification";
NSString *NSTextDidEndEditingNotification = @"NSTextDidEndEditingNotification";
NSString *NSTextDidChangeNotification = @"NSTextDidChangeNotification";

// NSTextStorage Notifications
NSString *NSTextStorageWillProcessEditingNotification =
  @"NSTextStorageWillProcessEditingNotification";
NSString *NSTextStorageDidProcessEditingNotification =
  @"NSTextStorageDidProcessEditingNotification";

// NSTextView notifications
NSString *NSTextViewDidChangeSelectionNotification =
@"NSTextViewDidChangeSelectionNotification";
NSString *NSTextViewWillChangeNotifyingTextViewNotification =
@"NSTextViewWillChangeNotifyingTextViewNotification";
NSString *NSTextViewDidChangeTypingAttributesNotification =
@"NSTextViewDidChangeTypingAttributesNotification";

// NSView notifications
NSString *NSViewFocusDidChangeNotification
    = @"NSViewFocusDidChangeNotification";
NSString *NSViewFrameDidChangeNotification
    = @"NSViewFrameDidChangeNotification";
NSString *NSViewBoundsDidChangeNotification
    = @"NSViewBoundsDidChangeNotification";
NSString *NSViewGlobalFrameDidChangeNotification
    = @"NSViewGlobalFrameDidChangeNotification";

// NSViewAnimation 
NSString *NSViewAnimationTargetKey     = @"NSViewAnimationTargetKey";
NSString *NSViewAnimationStartFrameKey = @"NSViewAnimationStartFrameKey";
NSString *NSViewAnimationEndFrameKey   = @"NSViewAnimationEndFrameKey";
NSString *NSViewAnimationEffectKey     = @"NSViewAnimationEffectKey";
NSString *NSViewAnimationFadeInEffect  = @"NSViewAnimationFadeInEffect";
NSString *NSViewAnimationFadeOutEffect = @"NSViewAnimationFadeOutEffect";


// NSMenu notifications
NSString* const NSMenuDidSendActionNotification = @"MenuDidSendAction";
NSString* const NSMenuWillSendActionNotification = @"MenuWillSendAction";
NSString* const NSMenuDidAddItemNotification = @"MenuDidAddItem";
NSString* const NSMenuDidRemoveItemNotification = @"MenuDidRemoveItem";
NSString* const NSMenuDidChangeItemNotification = @"MenuDidChangeItem";

// NSPopUpButton notification
NSString *NSPopUpButtonWillPopUpNotification = @"PopUpButtonWillPopUp";
NSString *NSPopUpButtonCellWillPopUpNotification = @"PopUpButtonCellWillPopUp";

// NSTable notifications
NSString *NSTableViewSelectionDidChangeNotification = @"TableViewSelectionDidChange";
NSString *NSTableViewColumnDidMoveNotification = @"TableViewColumnDidMove";
NSString *NSTableViewColumnDidResizeNotification = @"TableViewColumnDidResize";
NSString *NSTableViewSelectionIsChangingNotification = @"TableViewSelectionIsChanging";

// NSOutlineView notifications
NSString *NSOutlineViewSelectionDidChangeNotification = @"OutlineViewSelectionDidChange";
NSString *NSOutlineViewColumnDidMoveNotification = @"OutlineViewColumnDidMove";
NSString *NSOutlineViewColumnDidResizeNotification = @"OutlineViewColumnDidResize";
NSString *NSOutlineViewSelectionIsChangingNotification = @"OutlineViewSelectionIsChanging";
NSString *NSOutlineViewItemDidExpandNotification = @"OutlineViewItemDidExpand";
NSString *NSOutlineViewItemDidCollapseNotification = @"OutlineViewItemDidCollapse";
NSString *NSOutlineViewItemWillExpandNotification = @"OutlineViewItemWillExpand";
NSString *NSOutlineViewItemWillCollapseNotification = @"OutlineViewItemWillCollapse";

// NSWindow notifications
NSString *NSWindowDidBecomeKeyNotification = @"WindowDidBecomeKey";
NSString *NSWindowDidBecomeMainNotification = @"WindowDidBecomeMain";
NSString *NSWindowDidChangeScreenNotification = @"WindowDidChangeScreen";
NSString *NSWindowDidChangeScreenProfileNotification = @"NSWindowDidChangeScreenProfile";
NSString *NSWindowDidDeminiaturizeNotification = @"WindowDidDeminiaturize";
NSString *NSWindowDidEndSheetNotification = @"NSWindowDidEndSheet";
NSString *NSWindowDidExposeNotification = @"WindowDidExpose";
NSString *NSWindowDidMiniaturizeNotification = @"WindowDidMiniaturize";
NSString *NSWindowDidMoveNotification = @"WindowDidMove";
NSString *NSWindowDidResignKeyNotification = @"WindowDidResignKey";
NSString *NSWindowDidResignMainNotification = @"WindowDidResignMain";
NSString *NSWindowDidResizeNotification = @"WindowDidResize";
NSString *NSWindowDidUpdateNotification = @"WindowDidUpdate";
NSString *NSWindowWillBeginSheetNotification = @"NSWindowWillBeginSheet";
NSString *NSWindowWillCloseNotification = @"WindowWillClose";
NSString *NSWindowWillMiniaturizeNotification = @"WindowWillMiniaturize";
NSString *NSWindowWillMoveNotification = @"WindowWillMove";

// Workspace File Type Globals
NSString *NSPlainFileType = @"NSPlainFileType";
NSString *NSDirectoryFileType = @"NSDirectoryFileType";
NSString *NSApplicationFileType = @"NSApplicationFileType";
NSString *NSFilesystemFileType = @"NSFilesystemFileType";
NSString *NSShellCommandFileType = @"NSShellCommandFileType";

// Workspace File Operation Globals
NSString *NSWorkspaceCompressOperation = @"NSWorkspaceCompressOperation";
NSString *NSWorkspaceCopyOperation = @"NSWorkspaceCopyOperation";
NSString *NSWorkspaceDecompressOperation = @"NSWorkspaceDecompressOperation";
NSString *NSWorkspaceDecryptOperation = @"NSWorkspaceDecryptOperation";
NSString *NSWorkspaceDestroyOperation = @"NSWorkspaceDestroyOperation";
NSString *NSWorkspaceDuplicateOperation = @"NSWorkspaceDuplicateOperation";
NSString *NSWorkspaceEncryptOperation = @"NSWorkspaceEncryptOperation";
NSString *NSWorkspaceLinkOperation = @"NSWorkspaceLinkOperation";
NSString *NSWorkspaceMoveOperation = @"NSWorkspaceMoveOperation";
NSString *NSWorkspaceRecycleOperation = @"NSWorkspaceRecycleOperation";

// NSWorkspace notifications
NSString *NSWorkspaceDidLaunchApplicationNotification =
@"NSWorkspaceDidLaunchApplicationNotification";
NSString *NSWorkspaceDidMountNotification = @"NSWorkspaceDidMountNotification";
NSString *NSWorkspaceDidPerformFileOperationNotification =
@"NSWorkspaceDidPerformFileOperationNotification";
NSString *NSWorkspaceDidTerminateApplicationNotification =
@"NSWorkspaceDidTerminateApplicationNotification";
NSString *NSWorkspaceDidUnmountNotification =
@"NSWorkspaceDidUnmountNotification";
NSString *NSWorkspaceWillLaunchApplicationNotification =
@"NSWorkspaceWillLaunchApplicationNotification";
NSString *NSWorkspaceWillPowerOffNotification =
@"NSWorkspaceWillPowerOffNotification";
NSString *NSWorkspaceWillUnmountNotification =
@"NSWorkspaceWillUnmountNotification";
NSString *NSWorkspaceDidWakeNotification =
@"NSWorkspaceDidWakeNotification";
NSString *NSWorkspaceSessionDidBecomeActiveNotification =
@"NSWorkspaceSessionDidBecomeActiveNotification";
NSString *NSWorkspaceSessionDidResignActiveNotification =
@"NSWorkspaceSessionDidResignActiveNotification";
NSString *NSWorkspaceWillSleepNotification =
@"NSWorkspaceWillSleepNotification";

/*
 *	NSStringDrawing NSAttributedString additions
 */
NSString *NSAttachmentAttributeName = @"NSAttachmentAttributeName";
NSString *NSBackgroundColorAttributeName = @"NSBackgroundColorAttributeName";
NSString *NSBaselineOffsetAttributeName = @"NSBaselineOffsetAttributeName";
NSString *NSCursorAttributeName = @"NSCursorAttributeName";
NSString *NSExpansionAttributeName = @"NSExpansionAttributeName";
NSString *NSFontAttributeName = @"NSFontAttributeName";
NSString *NSForegroundColorAttributeName = @"NSForegroundColorAttributeName";
NSString *NSKernAttributeName = @"NSKernAttributeName";
NSString *NSLigatureAttributeName = @"NSLigatureAttributeName";
NSString *NSLinkAttributeName = @"NSLinkAttributeName";
NSString *NSObliquenessAttributeName = @"NSObliquenessAttributeName";
NSString *NSParagraphStyleAttributeName = @"NSParagraphStyleAttributeName";
NSString *NSShadowAttributeName = @"NSShadowAttributeName";
NSString *NSStrikethroughColorAttributeName
  = @"NSStrikethroughColorAttributeName";
NSString *NSStrikethroughStyleAttributeName = @"NSStrikethroughStyleAttributeName";
NSString *NSStrokeColorAttributeName = @"NSStrokeColorAttributeName";
NSString *NSStrokeWidthAttributeName = @"NSStrokeWidthAttributeName";
NSString *NSSuperscriptAttributeName = @"NSSuperscriptAttributeName";
NSString *NSToolTipAttributeName = @"NSToolTipAttributeName";
NSString *NSUnderlineColorAttributeName = @"NSUnderlineColorAttributeName";
NSString *NSUnderlineStyleAttributeName = @"NSUnderlineStyleAttributeName";

NSString *NSCharacterShapeAttributeName = @"CharacterShape";
NSString *NSGlyphInfoAttributeName = @"GlyphInfo";

NSString *NSPaperSizeDocumentAttribute = @"PaperSize";
NSString *NSLeftMarginDocumentAttribute = @"LeftMargin";
NSString *NSRightMarginDocumentAttribute = @"RightMargin";
NSString *NSTopMarginDocumentAttribute = @"TopMargin";
NSString *NSBottomMarginDocumentAttribute = @"BottomMargin";
NSString *NSHyphenationFactorDocumentAttribute = @"HyphenationFactor";
NSString *NSDocumentTypeDocumentAttribute = @"DocumentType";
NSString *NSCharacterEncodingDocumentAttribute = @"CharacterEncoding";
NSString *NSViewSizeDocumentAttribute = @"ViewSize";
NSString *NSViewZoomDocumentAttribute = @"ViewZoom";
NSString *NSViewModeDocumentAttribute = @"ViewMode";
NSString *NSBackgroundColorDocumentAttribute = @"BackgroundColor";
NSString *NSCocoaVersionDocumentAttribute = @"CocoaVersion";
NSString *NSReadOnlyDocumentAttribute = @"ReadOnly";
NSString *NSConvertedDocumentAttribute = @"Converted";
NSString *NSDefaultTabIntervalDocumentAttribute = @"DefaultTabInterval";
NSString *NSTitleDocumentAttribute = @"Title";
NSString *NSCompanyDocumentAttribute = @"Company";
NSString *NSCopyrightDocumentAttribute = @"Copyright";
NSString *NSSubjectDocumentAttribute = @"Subject";
NSString *NSAuthorDocumentAttribute = @"Author";
NSString *NSKeywordsDocumentAttribute = @"Keywords";
NSString *NSCommentDocumentAttribute = @"Comment";
NSString *NSEditorDocumentAttribute = @"Editor";
NSString *NSCreationTimeDocumentAttribute = @"CreationTime";
NSString *NSModificationTimeDocumentAttribute = @"ModificationTime";

const unsigned NSUnderlineByWordMask = 0x01;

NSString *NSSpellingStateAttributeName = @"NSSpellingState";
const unsigned NSSpellingStateSpellingFlag = 1;
const unsigned NSSpellingStateGrammarFlag = 2;


NSString *NSPlainTextDocumentType = @"PlainText";
NSString *NSRTFTextDocumentType = @"RTF";
NSString *NSRTFDTextDocumentType = @"RTFD";
NSString *NSMacSimpleTextDocumentType = @"SimpleText";
NSString *NSHTMLTextDocumentType = @"HTML";
NSString *NSDocFormatTextDocumentType = @"Doc";
NSString *NSWordMLTextDocumentType = @"WordML";

NSString *NSExcludedElementsDocumentAttribute = @"ExcludedElements";
NSString *NSTextEncodingNameDocumentAttribute = @"TextEncoding";
NSString *NSPrefixSpacesDocumentAttribute = @"PrefixSpaces";

NSString *NSBaseURLDocumentOption = @"BaseURL";
NSString *NSCharacterEncodingDocumentOption = @"CharacterEncoding";
NSString *NSDefaultAttributesDocumentOption = @"DefaultAttributes";
NSString *NSDocumentTypeDocumentOption = @"DocumentType";
NSString *NSTextEncodingNameDocumentOption = @"TextEncoding";
NSString *NSTextSizeMultiplierDocumentOption = @"TextSizeMultiplier";
NSString *NSTimeoutDocumentOption = @"Timeout";
NSString *NSWebPreferencesDocumentOption = @"WebPreferences";
NSString *NSWebResourceLoadDelegateDocumentOption = @"WebResourceLoadDelegate";

// NSTextTab
NSString *NSTabColumnTerminatorsAttributeName = @"TabColumnTerminatorsAttributeName"; 

// NSToolbar notifications
NSString *NSToolbarDidRemoveItemNotification = @"NSToolbarDidRemoveItemNotification";
NSString *NSToolbarWillAddItemNotification = @"NSToolbarWillAddItemNotification";

// NSToolbarItem constants
NSString *NSToolbarSeparatorItemIdentifier = @"NSToolbarSeparatorItemIdentifier";
NSString *NSToolbarSpaceItemIdentifier = @"NSToolbarSpaceItemIdentifier";
NSString *NSToolbarFlexibleSpaceItemIdentifier = @"NSToolbarFlexibleSpaceItemIdentifier";
NSString *NSToolbarShowColorsItemIdentifier = @"NSToolbarShowColorsItemIdentifier";
NSString *NSToolbarShowFontsItemIdentifier = @"NSToolbarShowFontsItemIdentifier";
NSString *NSToolbarCustomizeToolbarItemIdentifier = @"NSToolbarCustomizeToolbarItemIdentifier";
NSString *NSToolbarPrintItemIdentifier = @"NSToolbarPrintItemIdentifier";

/*
 * NSTextView userInfo for notifications 
 */
NSString *NSOldSelectedCharacterRange = @"NSOldSelectedCharacterRange";

/* NSFont matrix */
const CGFloat NSFontIdentityMatrix[] = {1, 0, 0, 1, 0, 0};

/* Drawing engine externs */
NSString *NSBackendContext = @"NSBackendContext";

typedef int NSWindowDepth;

/**** Color function externs ****/
/* Since these are constants it was not possible
   to do the OR directly.  If you change the
   _GS*BitValue numbers, please remember to
   change the corresponding depth values */
const NSWindowDepth _GSGrayBitValue = 256;
const NSWindowDepth _GSRGBBitValue = 512;
const NSWindowDepth _GSCMYKBitValue = 1024;
const NSWindowDepth _GSNamedBitValue = 2048;
const NSWindowDepth _GSCustomBitValue = 4096;
const NSWindowDepth NSDefaultDepth = 0;            // GRAY = 256, RGB = 512
const NSWindowDepth NSTwoBitGrayDepth = 258;       // 0100000010 GRAY | 2bps
const NSWindowDepth NSEightBitGrayDepth = 264;     // 0100001000 GRAY | 8bps
const NSWindowDepth NSEightBitRGBDepth = 514;      // 1000000010 RGB  | 2bps
const NSWindowDepth NSTwelveBitRGBDepth = 516;     // 1000000100 RGB  | 4bps
const NSWindowDepth GSSixteenBitRGBDepth = 517;    // 1000000101 RGB  | 5bps GNUstep specific
const NSWindowDepth NSTwentyFourBitRGBDepth = 520; // 1000001000 RGB  | 8bps
const NSWindowDepth _GSWindowDepths[7] = { 258, 264, 514, 516, 517, 520, 0 };

/* End of color functions externs */

// NSKeyValueBinding
NSString *NSObservedObjectKey = @"NSObservedObject";
NSString *NSObservedKeyPathKey = @"NSObservedKeyPath";
NSString *NSOptionsKey = @"NSOptions";

NSString *NSAllowsEditingMultipleValuesSelectionBindingOption = @"NSAllowsEditingMultipleValuesSelection";
NSString *NSAllowsNullArgumentBindingOption = @"NSAllowsNullArgument";
NSString *NSConditionallySetsEditableBindingOption = @"NSConditionallySetsEditable";
NSString *NSConditionallySetsEnabledBindingOption = @"NSConditionallySetsEnabled";
NSString *NSConditionallySetsHiddenBindingOption = @"NSConditionallySetsHidden";
NSString *NSContinuouslyUpdatesValueBindingOption = @"NSContinuouslyUpdatesValue";
NSString *NSCreatesSortDescriptorBindingOption = @"NSCreatesSortDescriptor";
NSString *NSDeletesObjectsOnRemoveBindingsOption = @"NSDeletesObjectsOnRemove";
NSString *NSDisplayNameBindingOption = @"NSDisplayName";
NSString *NSDisplayPatternBindingOption = @"NSDisplayPattern";
NSString *NSHandlesContentAsCompoundValueBindingOption = @"NSHandlesContentAsCompoundValue";
NSString *NSInsertsNullPlaceholderBindingOption = @"NSInsertsNullPlaceholder";
NSString *NSInvokesSeparatelyWithArrayObjectsBindingOption = @"NSInvokesSeparatelyWithArrayObjects";
NSString *NSMultipleValuesPlaceholderBindingOption = @"NSMultipleValuesPlaceholder";
NSString *NSNoSelectionPlaceholderBindingOption = @"NSNoSelectionPlaceholder";
NSString *NSNotApplicablePlaceholderBindingOption = @"NSNotApplicablePlaceholder";
NSString *NSNullPlaceholderBindingOption = @"NSNullPlaceholder";
NSString *NSPredicateFormatBindingOption = @"NSPredicateFormat";
NSString *NSRaisesForNotApplicableKeysBindingOption = @"NSRaisesForNotApplicableKeys";
NSString *NSSelectorNameBindingOption = @"NSSelectorName";
NSString *NSSelectsAllWhenSettingContentBindingOption = @"NSSelectsAllWhenSettingContent";
NSString *NSValidatesImmediatelyBindingOption = @"NSValidatesImmediately";
NSString *NSValueTransformerNameBindingOption = @"NSValueTransformerName";
NSString *NSValueTransformerBindingOption = @"NSValueTransformer";
 
NSString *NSAlignmentBinding = @"alignment";
NSString *NSContentArrayBinding = @"contentArray";
NSString *NSContentBinding = @"content";
NSString *NSContentObjectBinding = @"contentObject";
NSString *NSEditableBinding = @"editable";
NSString *NSEnabledBinding = @"enabled";
NSString *NSFontBinding = @"font";
NSString *NSFontNameBinding = @"fontName";
NSString *NSFontSizeBinding = @"fontSize";
NSString *NSHiddenBinding = @"hidden";
NSString *NSSelectedIndexBinding = @"selectedIndex";
NSString *NSSelectedObjectBinding = @"selectedObject";
NSString *NSSelectedTagBinding = @"selectedTag";
NSString *NSSelectionIndexesBinding = @"selectionIndexes";
NSString *NSTextColorBinding = @"textColor";
NSString *NSTitleBinding = @"title";
NSString *NSToolTipBinding = @"toolTip";
NSString *NSValueBinding = @"value";

// FIXME: Need to defined!
id NSMultipleValuesMarker = @"NSMultipleValuesMarker";
id NSNoSelectionMarker = @"NSNoSelectionMarker";
id NSNotApplicableMarker = @"NSNotApplicableMarker";


// NSNib
NSString *NSNibTopLevelObjects = @"NSTopLevelObjects";
NSString *NSNibOwner = @"NSOwner";

extern void __objc_gui_force_linking (void);

void
__objc_gui_force_linking (void)
{
  extern void __objc_gui_linking (void);
  __objc_gui_linking ();
}


