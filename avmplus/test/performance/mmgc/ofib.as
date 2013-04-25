/* -*- mode: java; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A somewhat nasty program.
 *
 * When run, ofib allocates 10946 new dynamic objects, and the largest
 * number of live objects (21 separate full structures) is 229866
 * objects.  Each object occupies at least 9 words plus 16 words for
 * the hash table's atoms table (the default capacity is 8), make it
 * 26 words for a total of 23906064 bytes.  This is the lower bound on
 * the amount of live data.  (Needs experimental verification, and
 * note the hash table size is subject to change.)
 *
 * The structure is entirely circular and utterly impervious to standard
 * reference counting - GC is needed to release it.
 *
 * The test is artificial and meant to stress the GC maximally, but if
 * you're charitable you can imagine the program as the abstraction of
 * one that manipulates a bunch of cached data structures where evicted
 * structures must be garbage collected.
 */

package
{
    import avmplus.*;

    function fib(n, pp) {
                var p = { parent: pp };
                if (n < 2)
                        return p;
                else {
                        p.x = fib(n-1, p);
                        p.y = fib(n-2, p);
                        return p;
                }
    }
    var iters = 70;
    if (System.argv.length > 0)
                iters = parseInt(System.argv[0]);
    print("Iterations: " + iters);
    var then = new Date();
    var res = new Array;
    for ( var i=0 ; i < iters ; i++ )
                res[i%20] = fib(20, null);
    var now = new Date();
    print("metric time " + (now - then));
}

