/* -*- mode: java; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A somewhat nasty program.
 *
 * When run, ofib-rc allocates 10946 new dynamic objects, and the largest
 * number of live objects (21 separate full structures) is 229866
 * objects.  Each object occupies at least 9 words plus 16 words for
 * the hash table's atoms table (the default capacity is 8), make it
 * 26 words for a total of 23906064 bytes.  This is the lower bound on
 * the amount of live data.  (Needs experimental verification, and
 * note the hash table size is subject to change.)
 *
 * The structure is entirely tree shaped and reference counting can
 * remove all of it without any need for GC - as long as the GC does
 * not intervene.
 *
 * The test is highly artificial and meant to stress the ZCT reaper
 * maximally, and the funny construction with the tmp array is the way
 * it is to trigger reaping.  Even so this test could be thwarted
 * if the GC is running at the time a reap should have been triggered.
 */
package
{
    import avmplus.*;

    function fib(n, pp) {
                var p = {};
                if (n < 2)
                        return p;
                else {
                        p.x = fib(n-1, p);
                        p.y = fib(n-2, p);
                        return p;
                }
    }
    var then = new Date();
    var res = new Array();
    var tmp = new Array();
    var i;
    for ( i=0 ; i < 500 ; i++ )
                tmp[i] = {};
    for ( i=0 ; i < 21 ; i++ )
                res[i] = fib(20, null);
    for ( i=0 ; i < 21 ; i++ )
                res[i] = null;
    for ( i=0 ; i < 500 ; i++ )
                tmp[i] = null;
    var now = new Date();
    print("metric time " + (now - then));
}

