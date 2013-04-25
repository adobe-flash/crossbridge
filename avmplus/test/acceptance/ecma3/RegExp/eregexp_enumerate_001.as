/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "regexp-enumerate-001";
//     var VERSION = "ECMA_2";
//     var TITLE   = "Regression Test for Enumerating Properties";
    var BUGNUMBER="339403";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    /*
     *  This test expects RegExp instances to have four enumerated properties:
     *  source, global, ignoreCase, and lastIndex
     *
     *  99.01.25:  now they also have a multiLine instance property.
     *
     */
    var r = new RegExp();
    var e = new Array();
    var t = new TestRegExp();

    for ( p in r ) {
        e[e.length] = { property:p, value:r[p] }; t.addProperty( p, r[p])
    };

    array[item++] = Assert.expectEq(
        "r = new RegExp(); e = new Array(); "+
        "for ( p in r ) { e[e.length] = { property:p, value:r[p] }; e.length",
        0,
        e.length );

    function TestRegExp() {
        this.addProperty = addProperty;
    }
    function addProperty(name, value) {
           var pass = false;

           if ( this.name != void 0 ) {
                pass = true;
           } else {
                this.name = false ;
           }

           testcases[testcases.length] = Assert.expectEq( 
                "Property: " + name +" already enumerated?",
                false,
                pass );

            if ( testcases[ testcases.length-1].passed == false ) {
                testcases[testcases.length-1].reason = "property already enumerated";

            }

    }

    return array;
}
