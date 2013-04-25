/* -*- mode: java; tab-width: 4 -*- */
/*
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import avmplus.*;

class Node
{
    var left:Node, right:Node;
    var i:int, j:int;
    function Node(l:Node = null, r:Node = null) { left = l; right = r; }
}

class GCBench
{
    var rootNode:Node;
                
    var tempTree:Node;
    var tStart:Number, tFinish:Number;
    var tElapsed:Number;

    static const kStretchTreeDepth:int    = 18;  // about 16Mb (for Java / C++)
    static const kLongLivedTreeDepth:int  = 16;  // about 4Mb (for Java / C++)
    static const kArraySize:int  = 500000;       // about 4Mb (for Java / C++)
    static const kMinTreeDepth:int = 4;
    static const kMaxTreeDepth:int = 16;

    static const bTrace:Boolean = true;

    function msg(s)
    {
        if (bTrace)
            trace(s);
    }

    function now()
    {
        return (new Date()).getTime();
    }

    function GCBench()
    {
        // msgBox = new TextField();
        // msgBox.width = stage.width
        // msgBox.height = stage.height

        msg("Garbage Collector Test");
        msg(" Stretching memory with a binary tree of depth " + kStretchTreeDepth);

        tStart = now();

        // Stretch the memory space quickly
        tempTree = MakeTree(kStretchTreeDepth);
        tempTree = null;

        // Create a long lived object
        msg(" Creating a long-lived binary tree of depth " + kLongLivedTreeDepth);
        var longLivedTree:Node = new Node();
        Populate(kLongLivedTreeDepth, longLivedTree);

        // Create long-lived array, filling half of it
        msg(" Creating a long-lived array of " + kArraySize + " doubles");
        var bcArray:Vector.<Number> = new Vector.<Number>(kArraySize);
        for (var bci:int = 0; bci < kArraySize/2; ++bci) {
            bcArray[bci] = 1.0/bci;
        }

        for (var d:int = kMinTreeDepth; d <= kMaxTreeDepth; d += 2) {
            TimeConstruction(d);
        }

        if (longLivedTree == null || bcArray[1000] != .001)
            trace("Failed");

        // fake reference to LongLivedTree
        // and array
        // to keep them from being optimized away

        tFinish = now();
        tElapsed = tFinish-tStart;
        trace("metric time "+tElapsed);

    }

    //function trace(str)
    //{
    //    msgBox.text += ( str + "\n");
    //}

    // Nodes used by a tree of a given size
    function TreeSize(ts:int):int
    {
        return ((1 << (ts + 1)) - 1);
    }

    // Number of iterations to use for a given tree depth
    function NumIters(ni:int):int
    {
        return Math.round(2 * TreeSize(kStretchTreeDepth) / TreeSize(ni));
    }

    // Build tree top down, assigning to older objects.
    function Populate(iDepth:int, thisNode:Node):void
    {
        if (iDepth<=0) {
            return;
        } else {
            iDepth--;
            thisNode.left  = new Node();
            thisNode.right = new Node();
            Populate (iDepth, thisNode.left);
            Populate (iDepth, thisNode.right);
        }
    }

    // Build tree bottom-up
    function MakeTree(iDepth:int):Node
    {
        if (iDepth<=0) {
            return new Node();
        } else {
            return new Node(MakeTree(iDepth-1),
                            MakeTree(iDepth-1));
        }
    }

    function TimeConstruction(depth:int):void
    {
        var rootNode:Node;
        var tStart:Number, tFinish:Number;
        var iNumIters:int = NumIters(depth);
        var tempTree:Node;

        msg("Creating " + iNumIters + " trees of depth " + depth);
        tStart = now();
        for (var i:int = 0; i < iNumIters; ++i) {
            tempTree = new Node();
            Populate(depth, tempTree);
            tempTree = null;
        }
        tFinish = now();
        msg("\tTop down construction took " + (tFinish - tStart) + "msecs");
        tStart = now();
        for (var itr:int = 0; itr < iNumIters; ++itr) {
            tempTree = MakeTree(depth);
            tempTree = null;
        }
        tFinish = now();
        msg("\tBottom up construction took " + (tFinish - tStart) + "msecs");
                
    }
}

(new GCBench());
