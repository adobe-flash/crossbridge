/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
*
* Date:    14 Jan 2002
* SUMMARY: Shouldn't crash on regexps with many nested parentheses
* See http://bugzilla.mozilla.org/show_bug.cgi?id=119909
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_119909";
// var VERSION = "";
// var TITLE   = "Shouldn't crash on regexps with many nested parentheses";
// var bug = "119909";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var NO_BACKREFS = false;
    var DO_BACKREFS = true;


    //--------------------------------------------------

    testThis(500, NO_BACKREFS, 'hello', 'goodbye');
    testThis(500, DO_BACKREFS, 'hello', 'goodbye');

    //--------------------------------------------------

    /*
     * Creates a regexp pattern like (((((((((hello)))))))))
     * and tests str.search(), str.match(), str.replace()
     */
    function testThis(numParens, doBackRefs, strOriginal, strReplace)
    {
      var openParen = doBackRefs? '(' : '(?:';
      var closeParen = ')';
      var pattern = '';

      for (var i=0; i<numParens; i++) {pattern += openParen;}
      pattern += strOriginal;
      for (i=0; i<numParens; i++) {pattern += closeParen;}

      try {
          var re = new RegExp(pattern);
    
          if (doBackRefs) {
          var res = strOriginal.search(re);
          array[item++] = Assert.expectEq( "strOriginal.search(re)", -1, res);

          res = strOriginal.match(re);
          array[item++] = Assert.expectEq( "strOriginal.match(re)", null, res);

          res = strOriginal.replace(re, strReplace);
          array[item++] = Assert.expectEq( "strOriginal.replace(re, strReplace)", "hello", res);
          } else {
          var res = strOriginal.search(re);
          array[item++] = Assert.expectEq( "strOriginal.search(re)", 0, res);

          res = strOriginal.match(re);
          //Get the first element to compare
          res = res[0];
          array[item++] = Assert.expectEq( "strOriginal.match(re)", 'hello', res);

          res = strOriginal.replace(re, strReplace);
          array[item++] = Assert.expectEq( "strOriginal.replace(re, strReplace)", "goodbye", res);
    
          }
      }
      catch (e: Error) {
          if (e.message.match("#1023"))
          array[item++] = Assert.expectEq( "str.search(re)", 0, 0);
          else
          throw(e);
      }
    }

    return array;
}
