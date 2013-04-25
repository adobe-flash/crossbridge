/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import com.adobe.test.Assert;

namespace ns = "some_ns"

interface IFoo
{
    function i1():*;
};

class Foo extends Object implements IFoo
{
    public var z:int;
    public function i1():* { return null; }
}

var f:Foo = new Foo();
var intf:IFoo = f;

var results = []
results.push({expected: true, actual: System.isGlobal(this)});
results.push({expected: false, actual: System.isGlobal(f)});
results.push({expected: false, actual: System.isGlobal(intf)});
results.push({expected: false, actual: System.isGlobal(1)});
results.push({expected: false, actual: System.isGlobal(true)});
results.push({expected: false, actual: System.isGlobal(1.0)});
results.push({expected: false, actual: System.isGlobal(ns)});
results.push({expected: false, actual: System.isGlobal("some_string")});
results.push({expected: false, actual: System.isGlobal({object_literal: 0})});
results.push({expected: false, actual: System.isGlobal([1,2,3])});

for (var i in results)
{
    var o = results[i]
    Assert.expectEq("test_"+i, o.expected, o.actual);
}

