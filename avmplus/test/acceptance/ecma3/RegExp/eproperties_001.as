/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
//     var SECTION = "RegExp/properties-001.js";
//     var VERSION = "ECMA_2";
//     var TITLE   = "Properties of RegExp Instances";
    var BUGNUMBER ="http://scopus/bugsplat/show_bug.cgi?id=346000";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    AddRegExpCases( new RegExp, "",   false, false, false, 0 );
    AddRegExpCases( /.*/,       ".*", false, false, false, 0 );
    AddRegExpCases( /[\d]{5}/g, "[\\d]{5}", true, false, false, 0 );
    AddRegExpCases( /[\S]?$/i,  "[\\S]?$", false, true, false, 0 );
    AddRegExpCases( /^([a-z]*)[^\w\s\f\n\r]+/m,  "^([a-z]*)[^\\w\\s\\f\\n\\r]+", false, false, true, 0 );
    AddRegExpCases( /[\D]{1,5}[\ -][\d]/gi,      "[\\D]{1,5}[\\ -][\\d]", true, true, false, 0 );
    AddRegExpCases( /[a-zA-Z0-9]*/gm, "[a-zA-Z0-9]*", true, false, true, 0 );
    AddRegExpCases( /x|y|z/gim, "x|y|z", true, true, true, 0 );

    AddRegExpCases( /\u0051/im, "\u0051", false, true, true, 0 );
    AddRegExpCases( /\x45/gm, "\\x45", true, false, true, 0 );
    AddRegExpCases( /\097/gi, "\\097", true, true, false, 0 );

    function AddRegExpCases( re, s, g, i, m, l ) {

        

        array[item++] = Assert.expectEq(
                     re + ".toString == RegExp.prototype.toString",
                     true,
                     re.toString == RegExp.prototype.toString );

        array[item++] = Assert.expectEq(
                     re + ".constructor == RegExp.prototype.constructor",
                     true,
                     re.constructor == RegExp.prototype.constructor );

        array[item++] = Assert.expectEq(
                     re + ".compile == RegExp.prototype.compile",
                     true,
                     re.compile == RegExp.prototype.compile );

        if (false) {
            array[item++] = Assert.expectEq(
                         re + ".test == RegExp.prototype.test",
                         true,
                         re.test == RegExp.prototype.test );
            
            array[item++] = Assert.expectEq(
                         re + ".exec == RegExp.prototype.exec",
                         true,
                         re.exec == RegExp.prototype.exec );
        }
        
        // properties

        array[item++] = Assert.expectEq(
                     re + ".source",
                     s,
                     re.source );
        try{
            re.source = "k";
        }catch(e:ReferenceError){
            thisError = e.toString();
        }finally{
            array[item++] = Assert.expectEq(
                     re + ".source='k'",
                     "ReferenceError: Error #1074",
                     Utils.referenceError(thisError) );
        }



        array[item++] = Assert.expectEq(
                     "delete"+re + ".source",
                     false,
                    delete re.source);



        array[item++] = Assert.expectEq(
                     re + ".toString()",
                     "/" + s +"/" + (g?"g":"") + (i?"i":"") +(m?"m":""),
                     re.toString() );

        array[item++] = Assert.expectEq(
                     re + ".global",
                     g,
                     re.global );

         try{
            re.global = true;
        }catch(e:ReferenceError){
             thisError = e.toString();
        }finally{
             array[item++] = Assert.expectEq(
                     re + ".global=true",
                     "ReferenceError: Error #1074",
                     Utils.referenceError(thisError) );
        }



        array[item++] = Assert.expectEq(
                     "delete"+re + ".global",
                     false,
                    delete re.global);

        array[item++] = Assert.expectEq(
                     re + ".ignoreCase",
                     i,
                     re.ignoreCase );

        try{
            re.ignoreCase = true;
        }catch(e:ReferenceError){
             thisError = e.toString();
        }finally{
             array[item++] = Assert.expectEq(
                     re + ".ignoreCase=true",
                     "ReferenceError: Error #1074",
                     Utils.referenceError(thisError) );
        }



        array[item++] = Assert.expectEq(
                     "delete"+re + ".ignoreCase",
                     false,
                    delete re.ignoreCase);

        array[item++] = Assert.expectEq(
                     re + ".multiline",
                     m,
                     re.multiline);

        try{
            re.multiline = true;
        }catch(e:ReferenceError){
             thisError = e.toString();
        }finally{
             array[item++] = Assert.expectEq(
                     re + ".multiline=true",
                     "ReferenceError: Error #1074",
                     Utils.referenceError(thisError) );
        }



        array[item++] = Assert.expectEq(
                     "delete"+re + ".multiline",
                     false,
                    delete re.multiline);

        array[item++] = Assert.expectEq(
                     re + ".lastIndex",
                     l,
                     re.lastIndex  );

        array[item++] = Assert.expectEq(
                     "delete"+re + ".lastIndex",
                     false,
                    delete re.lastIndex);
    }

    return array;
}
