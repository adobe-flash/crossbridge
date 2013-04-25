/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 04 September 2001
 *
 * SUMMARY: Regression test for Bugzilla bug 98306
 * "JS parser crashes in ParseAtom for script using Regexp()"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=98306
 */
//-----------------------------------------------------------------------------

// var SECTION = "eregress_98306";
// var VERSION = "";
// var TITLE   = "Testing that we don't crash on this code -";
// var bug = "98306";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var cnUBOUND = 10;
    var re;
    var s;


    s = '"Hello".match(/[/]/)';
    tryThis(s);

    s = 're = /[/';
    tryThis(s);

    s = 're = /[/]/';
    tryThis(s);

    s = 're = /[//]/';
    tryThis(s);

    // Try to provoke a crash -
    function tryThis(sCode)
    {
      var thisError = "no error";

      // sometimes more than one attempt is necessary -
      for (var i=0; i<cnUBOUND; i++)
      {
        try
        {
          sCode;
        }
        catch(e)
        {
          // do nothing; keep going -
          thisError = "error";
        }
      }

      array[item++] = Assert.expectEq( sCode, "no error", thisError);
    }

    return array;
}
