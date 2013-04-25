/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

import flash.sampler.*
import avmplus.*
import com.adobe.test.Assert;

// var SECTION = "Sampling";
// var VERSION = "AS3";
// var TITLE   = "Test the Invocation methods of Sampling api";

var isdebugger = System.isDebugger();


var objs:Array=new Array();
public class testclass {
    var string_instance:String;
    var int_instance:int;
    var bool_instance:Boolean;
    public var public_str:String;
    private var private_str:String;

    static var static_string:String;
    public static var public_static_string:String;
    private static var private_static_string:String;

    function testclass(s:String) {
        this.string_instance=s;
    }
    function func() {}
    public function publicfunction() {}
    private function privatefunction() {}
    private var _getset:int;
    public function get getset():int {
        return _getset;
    }
    public function set getset(n:int):void {
        _getset=n;
    }
    public function get getonly():int {
        return 10;
    }
    public function set setonly(n:int):void {
    }
}
function simpleLoop() {
    for (var i:int=0;i<10;i++) {
       objs.push(new testclass(""+i));
    }
}

sampleInternalAllocs(true);
startSampling();
simpleLoop();
pauseSampling();

var names1=getMemberNames(testclass,true);
var namestable:Object=new Object();

var items1:Array=[];
for each (var item1 in names1) {
    items1.push(item1.toString());
    namestable[item1.toString()]=item1;
}
items1.sort();
/*
print("getMemberNames,instanceNames=true");
for (var i=0;i<items1.length;i++) {
    print(items1[i]);
}
*/


if (isdebugger) {
    Assert.expectEq(
        "getMemberNamesWithInstancesLength: returns member names, instanceNames=true",
        15,
        items1.length
    );
    Assert.expectEq(
        'getMemberNamesWithInstancesLength: returns member names, instanceNames=true',
        'bool_instance,func,getonly,getset,http://adobe.com/AS3/2006/builtin::hasOwnProperty,http://adobe.com/AS3/2006/builtin::isPrototypeOf,http://adobe.com/AS3/2006/builtin::propertyIsEnumerable,int_instance,public_str,publicfunction,setonly,string_instance,testclass::_getset,testclass::private_str,testclass::privatefunction',
        items1.toString()
    );
} else {
    Assert.expectEq(
        "getMemberNamesWithInstancesLength: returns member names, instanceNames=true",
        0,
        items1.length
    );
    Assert.expectEq(
        'getMemberNamesWithInstancesLength: returns member names, instanceNames=true',
        '',
        items1.toString()
    );
}

var names2=getMemberNames(testclass,false);

var items2:Array=[];
for each (var item2 in names2) {
    items2.push(item2.toString());
}
items2.sort();
if (isdebugger) {
    Assert.expectEq(
        "getMemberNamesWithInstancesLength: returns member names, instanceNames=true",
        7,
        items2.length
    );
    Assert.expectEq(
        'getMemberNamesWithInstancesLength: returns member names, instanceNames=true',
        'http://adobe.com/AS3/2006/builtin::hasOwnProperty,http://adobe.com/AS3/2006/builtin::isPrototypeOf,http://adobe.com/AS3/2006/builtin::propertyIsEnumerable,prototype,public_static_string,static_string,testclass::private_static_string',
        items2.toString()
    );

    Assert.expectEq(
        "getInvocationCountConstructor",
        1,
        getInvocationCount(testclass,null)
    );
    Assert.expectEq(
        "getInvocationCountFunc0: function not called invocation is 0",
        0,
        getInvocationCount(testclass,namestable['func'])
    );
}
// call the function 4 times
objs[0].func();
objs[0].func();
objs[1].func();
objs[2].func();
if (isdebugger) {
    Assert.expectEq(
        "getInvocationCountFunc4: call function 4 times",
        4,
        getInvocationCount(testclass,namestable['func'])
    );
    Assert.expectEq(
        "getInvocationCountVar: instance variable returns -1 on invocation count",
        -1,
        getInvocationCount(testclass,namestable['public_str'])
    );
    
    Assert.expectEq(
        "isGetterSetterForBoth: isGetterSetter for function with get and set",
        true,
        isGetterSetter(testclass,namestable['getset'])
    );
    Assert.expectEq(
        "isGetterSetterSetOnly: isGetterSetter for function with set only",
        true,
        isGetterSetter(testclass,namestable['setonly'])
    );
    Assert.expectEq(
        "isGetterSetterGetOnly: isGetterSetter for function with get only",
        true,
        isGetterSetter(testclass,namestable['getonly'])
    );
    Assert.expectEq(
        "isGetterSetterInstanceVar: isGetterSetter for instance variable",
        false,
        isGetterSetter(testclass,namestable['public_str'])
    );
    Assert.expectEq(
        "isGetterSetterFunction: isGetterSetter for a standard function",
         false,
         isGetterSetter(testclass,namestable['func'])
    );
}
var value;
objs[0].getset=5;
value=objs[0].getset;
objs[1].getset=5;
value=objs[1].getset;
objs[1].setonly=5;
value=objs[1].getonly;
objs[0].setonly=5;
value=objs[0].getonly;

if (isdebugger) {
    Assert.expectEq(
        "getGetterInvocationCount: function getter and setter",
        2,
        getGetterInvocationCount(testclass,namestable['getset'])
    );
    Assert.expectEq(
        "getGetterInvocationCount: function setonly",
        -1,
        getGetterInvocationCount(testclass,namestable['setonly'])
    );
    Assert.expectEq(
        "getGetterInvocationCount: function getonly",
        2,
        getGetterInvocationCount(testclass,namestable['getonly'])
    );
    Assert.expectEq(
        "getSetterInvocationCount: function getter and setter",
        2,
        getSetterInvocationCount(testclass,namestable['getset'])
    );
    Assert.expectEq(
        "getSetterInvocationCount: function setonly",
        2,
        getSetterInvocationCount(testclass,namestable['setonly'])
    );
    Assert.expectEq(
        "getSetterInvocationCount: function getonly",
        -1,
        getSetterInvocationCount(testclass,namestable['getonly'])
    );
    Assert.expectEq(
        "getGetterInvocationCount: function",
        4, // ??
        getGetterInvocationCount(testclass,namestable['func'])
    );
    Assert.expectEq(
        "getSetterInvocationCount: function",
        4,  // ??
        getSetterInvocationCount(testclass,namestable['func'])
    );
    Assert.expectEq(
        "getSize: get size on getter/setter function",
        true,
        getSize(namestable['getset'])>0
    );
    Assert.expectEq(
        "getSize: get size on private instance variable",
        true,
        getSize(namestable['testclass::_getset'])>0
    );
}

}
