/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION="7.8.2";
//     var VERSION="ECMA_1"

    testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
{1
2}3     //This sentence is not valid ECMAScript sentence if semicolon not inserted         //automatically this sentence
        //will throw compiler error

    
    array[item++] = Assert.expectEq( "7.8.2 Semicolon is inserted automatically",      "passed","passed" );

var a:Number = 10;
var b:Number = 20;
var c:Number = 0;

function MyNumber(){
return
a+b
}

function MyAnotherNumber():Number{
return a+b
}

   array[item++] = Assert.expectEq( "7.8.2 Semicolon is inserted automatically",      true,MyNumber()!=MyAnotherNumber() );

a=b
++c
   array[item++] = Assert.expectEq( "7.8.2 Semicolon is inserted automatically",20,a );


       
    return ( array );
}

