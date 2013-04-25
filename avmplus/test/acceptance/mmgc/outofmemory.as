/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;

        /****  inner class Node  ***/
        class Node
        {
            var left:Node, right:Node;
            var i:int, j:int;
            function Node(l:Node = null, r:Node = null) { left = l; right = r; }
            
        }
        /**** end Node *******/


trace("before out-of-memory PASSED!");
var data:Array=new Array();
var last:Node=new Node();
var counter:int = 0;
var memWatcher:Number = 0;
while (true) {
    var node=new Node(last,last);
    data.push(node);
    last=node;
    counter++;
    if ((counter % 20000) == 0){
        if (System.totalMemory < memWatcher){
            trace("memory stopped growing, something wrong?");
            break;
        }
        memWatcher = System.totalMemory
    }
}

// if reaches this point did not run out of memory
// in order to run out of memory -memlimit 100 must be a runtime arg to the vm
trace("out-of-memory FAILED!");
