/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/* Acceptance test for rest array optimization.  We measure two
   programs, one where the analysis must definitely succeed and one
   where it ought to fail.  The ratio of the scores of the optimized
   program to those of the unoptimized program should be at least
   five.

   This acceptance test may start failing if:

    - the analysis gets broken
    - the analysis improves so much that the unoptimized program becomes optimized,
      this is not likely
    - allocation and object initialization speeds improve so much that
      rest array allocation starts to become affordable

   The test does not apply to builds with an installed debugger - the analysis
   is disabled if the debugger is present.
*/

// var SECTION = "Function";           // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";                // Version of ECMAScript or ActionScript
// var TITLE   = "rest optimiztation"; // Provide ECMA section title or a description
var BUGNUMBER = "569321";           // bugzilla.mozilla.org


class C {
    function push(...v):void {
        y = v.length;
    x = v[0];
    }
    function get length(): uint { return 0 }
    function set length(n: uint): void { }
    var x:*;
    var y:*;
}

function pushloopC():uint {
    var a:C = new C;
    for ( var i:int=0 ; i < 100000 ; i+=10 ) {
    a.length = 0;
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    }
    return a.length;
}

class D {
    function push(...v):void {
        w = v;
    x = w[0];
    }
    function get length(): uint { return 0 }
    function set length(n: uint): void { }
    var x:*;
    var w:*;
}

function pushloopD():uint {
    var a:D = new D;
    for ( var i:int=0 ; i < 100000 ; i+=10 ) {
    a.length = 0;
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    a.push(i);
    }
    return a.length;
}

function TEST(run:Function): Number {
    var then:Date = new Date();
    var iterations:int = 0;
    for (;;) {
    var now:Date = new Date();
    if (now.getTime() - then.getTime() > 1000.0)
        break;
    iterations++;
    run();
    }
    return iterations*(now.getTime()-then.getTime());
}

var m1 = TEST(pushloopC);
var m2 = TEST(pushloopD);

Assert.expectEq( "Rest array optimization (not enabled with the debugger!)", true, (m1/m2 > 5.0));

