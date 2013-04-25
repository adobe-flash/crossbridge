/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Proxy";
// var VERSION = "AS3";
// var TITLE   = "Proxy test ported from ATS: ATS9AS3 Automated/Proxy/deleteAndForInProxy";


// instantiate a new object
var myObject:* = new Object();

// wrap that object in a proxy
var myObjectProxy:ProxySmoke = new ProxySmoke(myObject);
var proxyOfProxy:ProxySmoke = new ProxySmoke(myObjectProxy);

myObjectProxy.property1 = Math.PI;
myObject.property2 = "Hello, world!";

Assert.expectEq("delete property on Object", true, delete myObject.property1);
Assert.expectEq("delete property on Proxy Object", true,
            delete myObjectProxy.property2);
Assert.expectEq("delete Nonexistent property on Object", true,
            delete myObject.nonExistent00);
Assert.expectEq("delete Nonexistent property on Proxy Object", true,
            delete myObjectProxy.nonExistent00);
Assert.expectEq("delete function on Object", true,
            delete myObject.delEchoFunction0);
Assert.expectEq("delete function on Proxy Object", true,
            delete myObjectProxy.delEchoFunction1);

myObject.delEchoFunction0 = function(myArg:String):String
{
    return myArg;
}
myObject.delEchoFunction1 = function(myArg:String):String
{
    return myArg;
}
myObject.echoString = function(myArg:String):String
{
    return myArg;
}

myObject.multiplyNumbers = function(myNum1:Number, myNum2:Number):Number
{
    return (myNum1 * myNum2);
}

myObject.sayHello = function():String
{
    return "Hello, World!";
}
function compareArray(a0:Array, a1:Array):int{
    var num = 0;
    trace("-------");
    for(var i in a1){
        trace(a0[i] + " / " + a1[i]);
        if(a0[i] != a1[i]){
            num++;
        }
    }
    return num;
}

myObject.testHasProperty1 = "weasel";
myObjectProxy.testHasProperty2 = "coffee";

// test inumerating over proxied object
function checkProxy(proxy:*, ansArray0:Array, ansArray1:Array):Boolean
{
    var pArray0 = new Array();
    var pArray1 = new Array();
    for(var i in proxy){
        pArray0.push(i);
        pArray1.push(proxy[i]);
    }
    pArray0.sort();
    pArray1.sort();
    trace("pArray0: " + pArray0.toString());
    trace("pArray1: " + pArray1.toString());
    var num0 = compareArray(pArray0, ansArray0);
    var num1 = compareArray(pArray0, ansArray0);
    if(num0 != 0 || num1 != 0){
        return false;
    }else{
        return true;
    }
}


var ansArray0 = new Array("delEchoFunction0","delEchoFunction1","echoString",
                          "multiplyNumbers","sayHello","testHasProperty1",
                          "testHasProperty2");
var ansArray1 = new Array("coffee","function Function() {}",
                          "function Function() {}","function Function() {}",
                          "function Function() {}","function Function() {}",
                          "weasel");

var obj = {firstName:"Tara", age:27, city:"San Francisco"};
var feProxy:ProxySmoke = new ProxySmoke(obj);
var feArray0 = new Array();
var ansFeArray0 = ["27", "San Francisco", "Tara"];
for each (var item in feProxy) {
    feArray0.push(item);
    //trace("item: " + item);
}
feArray0.sort();

Assert.expectEq("for in loop over Proxy Object variables", true,
            checkProxy(proxyOfProxy, ansArray0, ansArray1));
Assert.expectEq("for in loop over Proxy Object values", "spacer", "spacer");
Assert.expectEq("for each in loop over Proxy Object", 0,
            compareArray(feArray0, ansFeArray0));

Assert.expectEq("delete proxy of proxy Object", false, delete proxyOfProxy);
Assert.expectEq("delete proxy Object", false, delete feProxy);
Assert.expectEq("delete original Object", false, delete obj);

//proxy before and after then delete
var ansProxyArray0 = ["foo","num","str"];
var ansProxyArray1 = ["9999","nuts","yo mama"];
var obj1 = new Object();
obj1.str = "nuts";
var obj1Proxy0 = new ProxySmoke(obj1);
obj1Proxy0.num = 9999;
var obj1Proxy1 = new ProxySmoke(obj1);
obj1Proxy1.foo = "bar";
var obj1ProxyProxy1 = new ProxySmoke(obj1Proxy1);
obj1ProxyProxy1.foo = "yo mama";

Assert.expectEq("add properties one by one and for in on props", true,
            checkProxy(obj1ProxyProxy1, ansProxyArray0, ansProxyArray1));

var errorMsg = "no error";
try{
    obj1Proxy0.doAction();
}catch(e){
    errorMsg = e.toString().substring(0,"TypeError: Error #1006".length);
}

Assert.expectEq("call non existent function on proxy", "TypeError: Error #1006",
            errorMsg);

