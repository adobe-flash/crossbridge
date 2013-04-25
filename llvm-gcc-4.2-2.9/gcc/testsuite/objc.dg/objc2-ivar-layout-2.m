/* APPLE LOCAL file radar 5217964 - radar 5251019 */
/* Test that ivar layout bytestream info is generated as expected for weak as well
   as strong layout. */
/* { dg-do run { target powerpc*-*-darwin* i?86*-*-darwin* } } */
/* { dg-options "-mmacosx-version-min=10.5 -framework Foundation -fobjc-gc" } */
/* { dg-require-effective-target objc_gc } */

#include <objc/runtime.h>
#include <Foundation/Foundation.h>

@interface NSTableOptions : NSObject {
    id tableOptionsPanel;
    id rowsField;
    id rowsStepper;
    id columnsField;
    id columnsStepper;
    id mergeCellsButton;
    id splitCellsButton;
    id nestTableButton;
    id horizontalAlignment;
    id verticalAlignment;
    id borderField;
    id borderStepper;
    id borderColorWell;
    id backgroundPopUp;
    id backgroundColorWell;

    __weak id *_client;

    id *_table;
    long _level;
    long _tableRange;
    long _numRows;
    long _numCols;
    long _minRow;
    long _maxRow;
    long _minCol;
    long _maxCol;
    long _spacesOccupied;
    long _maxSpaces;
    id _cellRanges;

    id _defaultBorderColor;
    char _modifyingClient;
}
@end

@implementation NSTableOptions
@end

const char expected_strong [] = {0x0f, 0x01, 0x11, 0xa2, 0x10, 0};

const char expected_weak [] = {0xf0, 0x11, 0xe0, 0};

int main()
{
        int i = 0;
        const char *ivar_layout_string = (const char*)class_getIvarLayout([NSTableOptions class]);
        if (strcmp (ivar_layout_string, expected_strong))
          abort ();

	ivar_layout_string = (const char*)class_getWeakIvarLayout([NSTableOptions class]);
	if (strcmp (ivar_layout_string, expected_weak))
	  abort ();
        return 0;
}

