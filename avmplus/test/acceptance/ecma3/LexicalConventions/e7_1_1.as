/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "7.1-1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "White Space";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;

    // whitespace between var reserved word and identifier
    array[item++] = Assert.expectEq(  "Testing white space","var MYVAR1=10",'var'+' '+'MYVAR1'+'='+'10');

    // No-breakspace between var reserved word and identifier
    //array[item++] = Assert.expectEq(   "Testing No break space","var-·MYVAR1=10" ,'var'+'\u00A0'+'MYVAR1=10;MYVAR1');


    // tab between var  reserved word and identifier
	var		MYVAR1=10;
    array[item++] = Assert.expectEq(   'var'+'\t'+'MYVAR1=10;MYVAR1',   10, MYVAR1 );

	// newline between var reserved word and identifier
	var
MYVAR2=10;
    array[item++] = Assert.expectEq(   'var'+'\n'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );
   
    //backspace between var reserved word and   Result should be vaMYVAR2=10;MYVAR2 = 10     //PASSED!
    array[item++] = Assert.expectEq(   'var'+'\b'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    //vertical tabspace between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\v'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    //form feed between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\f'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );
    
    //carriage return between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\r'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    //double quotes between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\"'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    //single quote between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\''+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    //backslash between var reserved word and identifier
    array[item++] = Assert.expectEq(   'var'+'\\'+'MYVAR2=10;MYVAR2',   10, MYVAR2 );

    
   
    return ( array );
}
