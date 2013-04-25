/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.10.6.3";
//     var VERSION = "ECMA_1";
//     var TITLE   = "RegExp.prototype.test(string)"

    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
    var string = "mystring"

    array[item++] = Assert.expectEq( 
                                "01 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                RegExp.prototype.test(string) );

    var regexp = new RegExp("a|ab");
    var string = 'abc'

    array[item++] = Assert.expectEq( 
                                "02 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (regexp.test(string)) );

    var regexp = new RegExp("d|ef");
    var string = 'abc'

    array[item++] = Assert.expectEq( 
                                "03 ( RegExp.prototype.test(" + string + "))",
                                false ,
                                (regexp.test(string)) );


    var pattern = /((a)|(ab))((c)|(bc))/;
    var string = 'abc';

    array[item++] = Assert.expectEq( 
                                "04 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

     var pattern = /a[a-z]{2,4}/;
    var string = 'abcdefghi';

    array[item++] = Assert.expectEq( 
                                "05 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

    var pattern = /a[a-z]{2,4}?/;
    var string = 'abcdefghi';

    array[item++] = Assert.expectEq( 
                                "06 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

    var pattern = /(aa|aabaac|ba|b|c)*/;
    var string = 'aabaac';

    array[item++] = Assert.expectEq( 
                                "07 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

    var pattern = /^(a+)\1*,\1+$/;
    var string = 'aaaaaaaaaa,aaaaaaaaaaaaaaa';

    array[item++] = Assert.expectEq( 
                                "08 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

    var pattern = /(z)((a+)?(b+)?(c))*/;
    var string = 'zaacbbbcac';

    array[item++] = Assert.expectEq( 
                                "09 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

     var pattern = /(a*)*/;
    var string = 'b';

    array[item++] = Assert.expectEq( 
                                "10 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

     var pattern = /(a*)b\1+/;
    var string = 'baaaac';

    array[item++] = Assert.expectEq( 
                                "11 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

    var pattern = /(?=(a+))/;
    var string = 'baaabac';

    array[item++] = Assert.expectEq( 
                                "12 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );

     var pattern = /(?=(a+))a*b\1/;
    var string = 'baaabac';

    array[item++] = Assert.expectEq( 
                                "13 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );





    var pattern = /(.*?)a(?!(a+)b\2c)\2(.*)/;
    var string = 'baaabac';

    array[item++] = Assert.expectEq( 
                                "14 ( RegExp.prototype.test(" + string + "))",
                                true ,
                                (pattern.test(string)) );






    return ( array );
}
