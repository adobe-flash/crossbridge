/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


        /****  inner class Node  ***/
        class Node
        {
            var left:Node, right:Node;
            var i:int, j:int;
            function Node(l:Node = null, r:Node = null) { left = l; right = r; }
            
        }
        /**** end Node *******/

trace("starting memlimit test");
var data:Array=new Array();
var last:Node=new Node();
for (var i=0;i<100000;i++) {
    var node=new Node(last,last);
    data.push(node);
    last=node;
}

trace("memlimit not exceeded PASSED!");
