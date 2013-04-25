/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System
import com.adobe.test.Assert;

function parse(s:String):String
{
    try
    {
        return XML(s).toXMLString();
    }
    catch(e)
    {
        var str:String = String(e);
        var numIndex = str.indexOf("Error #");
        return str.substr(numIndex, 11);
    }
}


    XML.prettyPrinting = false;
    XML.prettyIndent = 0;
    
    var results = []

    var s:String = "foo"
    var r:String = parse(s);
    results.push({expected: "foo", actual: r});

    var s:String = "<one>1</one>"
    var r:String = parse(s);
    results.push({expected: "<one>1</one>", actual: r});

    var s:String = "foo" + String.fromCharCode(0) + "<one>1</one>"
    var r:String = parse(s);
    if (System.swfVersion >= 11)
        results.push({expected: "Error #1088", actual: r});
    else
        results.push({expected: "foo", actual: r});

    var s:String = "foo" + String.fromCharCode(0) + "??garbled randomness that is not legal XML!"
    var r:String = parse(s);
    if (System.swfVersion >= 11)
        results.push({expected: "foo&#x0;??garbled randomness that is not legal XML!", actual: r});
    else
        results.push({expected: "foo", actual: r});
    
    var s:String = String.fromCharCode(0) + "<one>1</one>"
    var r:String = parse(s);
    if (System.swfVersion >= 11)
        results.push({expected: "Error #1088", actual: r});
    else
        results.push({expected: "", actual: r});

    var s:String = "<head><one>1</one>" + String.fromCharCode(0) + "<two>2</two></head>"
    var r:String = parse(s);
    if (System.swfVersion >= 11)
        results.push({expected: "<head><one>1</one>&#x0;<two>2</two></head>", actual: r});
    else
        results.push({expected: "Error #1085", actual: r});


for (var i in results)
{
    var o = results[i]
    Assert.expectEq("test_"+i, o.expected.toString(), o.actual.toString());
}

