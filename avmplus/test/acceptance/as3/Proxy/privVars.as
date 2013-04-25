/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Proxy";
// var VERSION = "AS3";
// var TITLE   = "Proxy test ported from ATS: ATS9AS3 Automated/Proxy/privVars";


var obj = new Animal();
obj.name = "Tiger";
obj.internalBool = false;
var proxyObj = new PrivProxy(obj);
proxyObj.age = 254;

Assert.expectEq("proxy public var", "Tiger", proxyObj.name);
Assert.expectEq("proxy private var", undefined, proxyObj.privVar);
Assert.expectEq("proxy protected var", undefined, proxyObj.protVar);
Assert.expectEq("proxy Number defined in Class var", 999999, proxyObj.insideNum);
Assert.expectEq("proxy internal var", false, proxyObj.internalBool);
Assert.expectEq("proxy staticStr on proxyObj", undefined, proxyObj.staticStr);
Assert.expectEq("proxy prototype", "Proto Shark", proxyObj.protoVar);
Assert.expectEq("proxy original object.age", 254, obj.age);

proxyObj.insideNum = -1234;
proxyObj.age = 254;

Assert.expectEq("proxy overwritten inside num", -1234, obj.insideNum);

proxyObj.staticStr = "Lion";
proxyObj.dynNull = "not Null";

Assert.expectEq("proxy  - trace static", "Panda", Animal.staticStr);
Assert.expectEq("proxy  - dyanmic", "not Null", obj.dynNull);

obj.dynNull = null;

Assert.expectEq("proxy  - dyanmic  as null", null, proxyObj.dynNull);

obj.func = function(){ }

Assert.expectEq("call function with 3 number arguments",
            "Method func was called. With args: Monkey,9999,true",
            proxyObj.func("Monkey", 9999, true));
Assert.expectEq("call function with 6 number arguments",
            "Method func was called. With args: Monkey,9999,true,Monkey,9999,true",
            proxyObj.func("Monkey", 9999, true, "Monkey", 9999, true));

