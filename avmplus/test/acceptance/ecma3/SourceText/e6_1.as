/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "6-1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Source Text";


    var testcases = getTestCases();
    
function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq( 
                                    "// the following character should not be interpreted as a line terminator in a comment: \u000A",
                                    'PASSED',
                                    "PASSED" );

    // \u000A array[item++].actual = "FAILED!";

    array[item++] = Assert.expectEq( 
                                    "// the following character should not be interpreted as a line terminator in a comment: \\n 'FAILED'",
                                    'PASSED',
                                   'PASSED' );

    // the following character should not be interpreted as a line terminator: \\n array[item++].actual = "FAILED"

    array[item++] = Assert.expectEq( 
                                    "// the following character should not be interpreted as a line terminator in a comment: \\u000A 'FAILED'",
                                    'PASSED',
                                    'PASSED' )

    // the following character should not be interpreted as a line terminator:   \u000A array[item++].actual = "FAILED"

    array[item++] = Assert.expectEq( 
                                    "// the following character should not be interpreted as a line terminator in a comment: \n 'PASSED'",
                                    'PASSED',
                                    'PASSED' )
    // the following character should not be interpreted as a line terminator: \n array[item++].actual = 'FAILED'

    array[item++] = Assert.expectEq(   
                                    "// the following character should not be interpreted as a line terminator in a comment: u000D",
                                    'PASSED',
                                    'PASSED' )

    // the following character should not be interpreted as a line terminator:   \u000D array[item++].actual = "FAILED"

    array[item++] = Assert.expectEq(   
                                    "// the following character should not be interpreted as a line terminator in a comment: u2028",
                                    'PASSED',
                                    'PASSED' )

    // the following character should not be interpreted as a line terminator:   \u2028 array[item++].actual = "FAILED"

    array[item++] = Assert.expectEq(   
                                    "// the following character should not be interpreted as a line terminator in a comment: u2029",
                                    'PASSED',
                                    'PASSED' )

    // the following character should not be interpreted as a line terminator:   \u2029 array[item++].actual = "FAILED"

    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  backspace in a string: \u0008",'abc\bdef',
                                    'abc\u0008def' )
    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  whitespace(Tab) in a string: \u0009",'abc\tdef',
                                    'abc\u0009def' )
    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  vertical tab in a string: \u000B",'abc\vdef',
                                    'abc\u000Bdef' )

    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  form feed in a string: \u000C",'abc\fdef','abc\u000Cdef' )
  

    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  double quotes in a string and should not terminate a string but should just add a character to the string: \u0022",'abc\"def',
                                    'abc\u0022def' )

    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  single quote in a string and should not terminate a string but should just add a character to the string : \u0027",'abc\'def',
                                    'abc\u0027def' )

    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  single blackslash in a string : \u005C",'abc\\def','abc\u005Cdef' )
  
    array[item++] = Assert.expectEq(   
                                    "the following character should  be interpreted as  space in a string : \u0020",'abc def','abc\u0020def' )


    return array;
}
