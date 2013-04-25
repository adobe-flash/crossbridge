/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var nodeCount:int = 1000;
var referenceResult:String = usingAppendChildAndE4X(nodeCount).toXMLString();

if (CONFIG::desktop) {
    var startTime:Number = new Date();
    var result:XML = addingToXMLList(nodeCount);
    var totaltime:Number = new Date() - startTime;
} else {
    var startTime:Number = getTimer();
    var result:XML = addingToXMLList(nodeCount);
    var totaltime:Number = getTimer() - startTime;
}

if (referenceResult != result.toXMLString()) {
    print("ERROR! XML does not match reference");
} else {
    print('metric time '+totaltime);
}

function usingAppendChildAndE4X(nodeCount:int):XML
{
    var xml:XML = <root />;

    for (var i:int = 0; i < nodeCount; i++) {
        xml.appendChild(<node id={i} />);
    }

    return xml;
}

function addingToXMLList(nodeCount:int):XML
{
    var xml:XML = <root />;
    var nodes:XMLList = new XMLList();

    for (var i:int = 0; i < nodeCount; i++) {
        nodes += <node id={i} />;
    }

    xml.setChildren(nodes);

    return xml;
}
