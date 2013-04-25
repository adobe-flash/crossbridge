/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "LexicalConventions/regexp-literals-002.js";
//     var VERSION = "ECMA_2";
//     var TITLE   = "Regular Expression Literals";

    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;
    // A regular expression literal represents an object of type RegExp.

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x*/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x*y/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy*yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/.{1}/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/.{3,4}/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/ab{0,}bc/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy+yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy{1,}yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy{1,3}yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy?yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/$/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x.*y/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x[yz]m/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x[^yz]m/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x[^-z]m/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/x\yz\y/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a\Sb/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/\d/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/\D/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/xy|yz/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/()ef/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a\(b/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a\\b/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a)b(c)/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a+b+c/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a.+?c/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a+|b*)/ instanceof RegExp).toString() );


    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a+|b){0,}/ instanceof RegExp).toString() );

       
    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a+|b)+/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a+|b)?/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a+|b){0,1}/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/[^ab]*/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/([abc])*d/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/a|b|c|d|e/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(a|b|c|d|e)f/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/[a-zA-Z_][a-zA-Z0-9_]*/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/^a(bc+|b[eh])g|.h$/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(bc+d$|ef*g.|h?i(j|k))/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/((((((((((a))))))))))\10/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/(.*)c(.*)/ instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/abc/i instanceof RegExp).toString() );

    array[item++] = Assert.expectEq(
        "// A regular expression literal represents an object of type RegExp.",
        "true",
        (/ab{0,1}?c/i instanceof RegExp).toString() );
        
    return array;
}
