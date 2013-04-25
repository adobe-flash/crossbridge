/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "7.7.3-1";
// var VERSION = "ECMA_1";
// var TITLE   = "Numeric Literals";
var BUGNUMBER="122877";


var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;
    
    array[item++] = Assert.expectEq( 
        "0x12345678",
        305419896,
        0x12345678 );
    
    array[item++] = Assert.expectEq( 
        "0x80000000",
        2147483648,
        0x80000000 );
    
    array[item++] = Assert.expectEq( 
        "0xffffffff",
        4294967295,
        0xffffffff );
    
    array[item++] = Assert.expectEq( 
        "0x100000000",
        4294967296,
        0x100000000 );
    
    // we will not support octal for flash 8
    /*array[item++] = Assert.expectEq( 
        "077777777777777777",
        2251799813685247,
        077777777777777777 );
    
    array[item++] = Assert.expectEq( 
        "077777777777777776",
        2251799813685246,
        077777777777777776 );
    */
    array[item++] = Assert.expectEq( 
        "0x1fffffffffffff",
        9007199254740991,
        0x1fffffffffffff );
    
    array[item++] = Assert.expectEq( 
        "0x20000000000000",
        9007199254740992,
        0x20000000000000 );
    
    array[item++] = Assert.expectEq( 
        "0x20123456789abc",
        9027215253084860,
        0x20123456789abc );
    
    array[item++] = Assert.expectEq( 
        "0x20123456789abd",
        9027215253084860,
        0x20123456789abd );
    
    array[item++] = Assert.expectEq( 
        "0x20123456789abe",
        9027215253084862,
        0x20123456789abe );
    
    array[item++] = Assert.expectEq( 
        "0x20123456789abf",
        9027215253084864,
        0x20123456789abf );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000080",
        1152921504606847000,
        0x1000000000000080 );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000081",
        1152921504606847200,
        0x1000000000000081 );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000100",
        1152921504606847200,
        0x1000000000000100 );
    
    array[item++] = Assert.expectEq( 
        "0x100000000000017f",
        1152921504606847200,
        0x100000000000017f );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000180",
        (1152921504606847500),
        0x1000000000000180);
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000181",
        1152921504606847500,
        0x1000000000000181 );
    
    array[item++] = Assert.expectEq( 
        "0x10000000000001f0",
        1152921504606847500,
        0x10000000000001f0 );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000200",
        1152921504606847500,
        0x1000000000000200 );
    
    array[item++] = Assert.expectEq( 
        "0x100000000000027f",
        1152921504606847500,
        0x100000000000027f );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000280",
        1152921504606847500,
        0x1000000000000280 );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000281",
        1152921504606847700,
        0x1000000000000281 );
    
    array[item++] = Assert.expectEq( 
        "0x10000000000002ff",
        1152921504606847700,
        0x10000000000002ff );
    
    array[item++] = Assert.expectEq( 
        "0x1000000000000300",
        1152921504606847700,
        0x1000000000000300 );
    
    array[item++] = Assert.expectEq( 
        "0x10000000000000000",
        18446744073709552000,
        0x10000000000000000 );

    return array;
}
