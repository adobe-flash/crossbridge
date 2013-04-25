/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
* Date: 26 November 2000
*
*
*SUMMARY: Passing a RegExp object to a RegExp() constructor.
*This test arose from Bugzilla bug 61266. The ECMA3 section is:
*
*  15.10.4.1 new RegExp(pattern, flags)
*
*  If pattern is an object R whose [[Class]] property is "RegExp" and
*  flags is undefined, then let P be the pattern used to construct R
*  and let F be the flags used to construct R. If pattern is an object R
*  whose [[Class]] property is "RegExp" and flags is not undefined,
*  then throw a TypeError exception. Otherwise, let P be the empty string
*  if pattern is undefined and ToString(pattern) otherwise, and let F be
*  the empty string if flags is undefined and ToString(flags) otherwise.
*
*
*The current test will check the first scenario outlined above:
*
*   "pattern" is itself a RegExp object R
*   "flags"  is undefined
*
* We check that a new RegExp object obj2 defined from these parameters
* is morally the same as the original RegExp object obj1. Of course, they
* can't be equal as objects - so we check their enumerable properties...
*
* In this test, the initial RegExp obj1 will include a flag. The flags
* parameter for obj2  will be undefined in the sense of not being provided.
*/
//-------------------------------------------------------------------------------------------------

// var SECTION = "e15_10_4_1_3";
// var VERSION = "";
// var TITLE   = "Passing a RegExp object to a RegExp() constructor";
// var bug = "61266";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var statprefix = 'Applying RegExp() twice to pattern ';
    var statmiddle = ' and flag ';
    var statsuffix =  '; testing property ';
    var singlequote = "'";
    var i = -1; var j = -1; var s = '';
    var obj1 = null; var obj2 = null;
    var status = ''; var actual = ''; var expect = ''; var msg = '';
    var patterns = new Array();
    var flags = new Array();


    // various regular expressions to try -
    patterns[0] = '';
    patterns[1] = 'abc';
    patterns[2] = '(.*)(3-1)\s\w';
    patterns[3] = '(.*)(...)\\s\\w';
    patterns[4] = '[^A-Za-z0-9_]';
    patterns[5] = '[^\f\n\r\t\v](123.5)([4 - 8]$)';

    // various flags to try -
    flags[0] = 'i';
    flags[1] = 'g';
    flags[2] = 'm';
    flags[3] = undefined;


    for (i in patterns)
    {
      s = patterns[i];

      for (j in flags)
      {
        f = flags[j];
        status = getStatus(s, f);
        obj1 = new RegExp(s, f);
        obj2 = new RegExp(obj1);

        msg  = status + quote("dotall");
        actual = obj2.dotall;
        expect = obj1.dotall;
        array[item++] = Assert.expectEq( msg, expect, actual);

        msg  = status + quote("extended");
        actual = obj2.extended;
        expect = obj1.extended;
        array[item++] = Assert.expectEq( msg, expect, actual);

        msg  = status + quote("ignoreCase");
        actual = obj2.ignoreCase;
        expect = obj1.ignoreCase;
        array[item++] = Assert.expectEq( msg, expect, actual);

        msg  = status + quote("lastIndex");
        actual = obj2.lastIndex;
        expect = obj1.lastIndex;
        array[item++] = Assert.expectEq( msg, expect, actual);

        msg  = status + quote("multiline");
        actual = obj2.multiline;
        expect = obj1.multiline;
        array[item++] = Assert.expectEq( msg, expect, actual);

        msg  = status + quote("source");
        actual = obj2.source;
        expect = obj1.source;
        array[item++] = Assert.expectEq( msg, expect, actual);
      }
    }


    function getStatus(regexp, flag)
    {
      return (statprefix  +  quote(regexp) +  statmiddle  +  flag  +  statsuffix);
    }


    function quote(text)
    {
      return (singlequote  +  text  + singlequote);
    }

    return array;
}
