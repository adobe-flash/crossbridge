/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
try {
    import avmplus.* ;      // System class in the avmshell
} catch (e) {
    import flash.system.* ; // System class in the flash player
import com.adobe.test.Assert;
}

// var SECTION = "System::pauseForGCIfCollectionImminent";
// var VERSION = "";
// var TITLE = "Test pauseForGCIfCollectionImminent api";


class Node {
    var left: Node, right: Node;
    var i: int, j: int;

    function Node(l: Node = null,
    r: Node = null) {
        left = l;
        right = r;
    }
}

// DO NOT MESS WITH THE PARAMETER VALUES.
//
// If the test takes to long to run on mobile with these values then either set the values
// differently on mobile, or disable the test on mobile.  On desktop we need a decent
// number of iterations in order to be able to run reliably, the test is statistical and
// sometimes getting a hit is tricky on a fast machine.

var i_loops = 1000;
var j_loops = 1000;

var start = new Date();
var fractions = [Number.MIN_VALUE, 0, -0,  0.25, 0.5, 0.75, 1.0];
for each(var f in fractions) {
    trace("starting pauseForGCIfCollectionImminent test @ " + f);

    var hits = 0;
    var a = [];
    for (var i = 0; i < i_loops; i++) {
        for (var j = 0; j < j_loops; j++)
            a[0] = 3.14159 + i * j; // compute a Number and store it in a * location to box it

        // Bugzilla 685161: Invoking a System method can disrupt
        // observed memory if its MethodSignature had been evicted but
        // weakly-held until some GC during loop freed it and cleared
        // the weak ref.
        //
        // As work-around, pre-call pauseForGCIfCollectionImminent
        // with imminence=1.0 (which should be a no-op) and the two
        // getters, ensuring all MethodSignatures are available before
        // memory observations in critical section.
        System.pauseForGCIfCollectionImminent(1.0);
        System.totalMemory;
        System.freeMemory;

        // Start critical section
        var beforeTotal = System.totalMemory;
        var beforeFree = System.freeMemory;
        System.pauseForGCIfCollectionImminent(f);
        var afterTotal = System.totalMemory;
        var afterFree = System.freeMemory;
        // End of critical section

        // Bugzilla 678975: 'total' once denoted used+free; now 'total' is used
        var beforeUsed = beforeTotal;
        var afterUsed = afterTotal;
        if (afterUsed < beforeUsed)
            hits++;
        if (((i + 1) % 100) == 0)
            trace("Hits: " + hits);
    }

    Assert.expectEq("pauseForGCIfCollectionImminent test f=" + f + " : hits="+hits,
                true,
                ((hits > 0) == (f < 1.0)));
}
trace('Main tests: '+(new Date() - start))


start = new Date();
var error_cases = [NaN, undefined, Number.NEGATIVE_INFINITY,
                   -1, 1.000000001, 10000, Number.MAX_VALUE,
                   Number.POSITIVE_INFINITY];

for each (var ec in error_cases) {
    error = "no error";
    try {
        System.pauseForGCIfCollectionImminent(ec);
    } catch (err) {
        error = err.toString();
    }
    Assert.expectEq("pauseForGCIfCollectionImminent test @ " + ec,
                "no error", error);
}
var loop_time = new Date() - start;
trace("error tests: "+loop_time);

