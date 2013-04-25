/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "Proxy";
// var VERSION = "AS3";
// var TITLE   = "Proxy advanced tests ported from ATS: ATS9AS3 Automated/Proxy/testProxyAdvanced";


// This tests getting and setting property

// instantiate a new object
var myObject:Object = new Object();

// wrap that object in a proxy
var myObjectProxy:ProxyAdvanced = new ProxyAdvanced(myObject);

// simulate addProperty
function getQuantity():Number
{
    return books;
}

function setQuantity(numBooks:Number):void
{
    books = numBooks;
}

function getTitle():String
{
    return "Catcher in the Rye";
}

var myObjectProxy2:Book = new Book();

myObjectProxy2.addProperty("bookCount", getQuantity, setQuantity);
myObjectProxy2.addProperty("bookName", getTitle, null);

myObjectProxy2.bookCount = 5;


Assert.expectEq("Test getter/setter using 'addProperty'",
            "You ordered 5 copies of the book",
            "You ordered " + myObjectProxy2.bookCount + " copies of the book");
Assert.expectEq("Test getter with null setter using 'addProperty'",
            "The book you ordered is Catcher in the Rye",
            "The book you ordered is " + myObjectProxy2.bookName);

// Test array operator

myObjectProxy.testArrayOperator = 555;
Assert.expectEq("Test getter with array operator", 555,
            myObjectProxy["testArrayOperator"]);

myObjectProxy["testArrayOperator"] = 111;
Assert.expectEq("Test setter with array operator", 111,
            myObjectProxy.testArrayOperator);

import NameSpacePackage.*;
// Test namespaces:
import com.adobe.test.Assert;
var eg:NameSpaceExample = new NameSpaceExample();
var p:ProxyAdvanced = new ProxyAdvanced(eg);


eg.NameSpaceDef::two = "two";




// using myClass.ns::prop
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
            "eg.NameSpaceDef::two = two",
            'eg.NameSpaceDef::two = ' + eg.NameSpaceDef::two);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
            "p.NameSpaceDef::two = two",
            'p.NameSpaceDef::two = ' + p.NameSpaceDef::two);

p.NameSpaceDef::two = "twenty-two";

Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
            "eg.NameSpaceDef::two = twenty-two",
            'eg.NameSpaceDef::two = ' + eg.NameSpaceDef::two);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
            "p.NameSpaceDef::two = twenty-two",
            'p.NameSpaceDef::two = ' + p.NameSpaceDef::two);

// wrap use namespace inside a function so the scope of the
// use namespace declaration is only scoped to the function.
function doUseNameSpace()
{
    use namespace NameSpaceDef;
    answerArr = []
    try {
        eg.three = 3;
        answerArr[0] = 'eg.NameSpaceDef::three = ' + eg.NameSpaceDef::three;
        answerArr[1] = 'p.NameSpaceDef::three = ' + p.NameSpaceDef::three;
        answerArr[2] = 'eg.three = ' + eg.three;
        answerArr[3] = 'p.three = ' + p.three;
    
        p.three = 33;
    
        answerArr[4] = 'eg.NameSpaceDef::three = ' + eg.NameSpaceDef::three;
        answerArr[5] = 'p.NameSpaceDef::three = ' + p.NameSpaceDef::three;
        answerArr[6] = 'eg.three = ' + eg.three;
        answerArr[7] = 'p.three = ' + p.three;
    } catch (notUsingNameSpace) {
        answerArr[0] = 'Bug 174012';
        answerArr[1] = 'Bug 174012';
        answerArr[2] = 'Bug 174012';
        answerArr[3] = 'Bug 174012';
        answerArr[4] = 'Bug 174012';
        answerArr[5] = 'Bug 174012';
        answerArr[6] = 'Bug 174012';
        answerArr[7] = 'Bug 174012';
    }
    Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
                "eg.NameSpaceDef::three = 3",
                answerArr[0]);
    Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
                "p.NameSpaceDef::three = 3",
                answerArr[1]);
    Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.myProp'",
                "eg.three = 3",
                answerArr[2]);
    Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.myProp'",
                "p.three = 3",
                answerArr[3]);

    Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
                "eg.NameSpaceDef::three = 33",
                answerArr[4]);
    Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.ns::myProp'",
                "p.NameSpaceDef::three = 33",
                answerArr[5]);
    Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'myClass.myProp'",
                "eg.three = 33",
                answerArr[6]);
    Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'myClass.myProp'",
                "p.three = 33",
                answerArr[7]);
}
doUseNameSpace();

eg.NameSpaceDef::one = "Namespace one";

eg.onePub = "Public one";


// namespace public and namespace ns
// after setting value on eg
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'eg.ns::one'",
            "eg.NameSpaceDef::one = Namespace one",
            'eg.NameSpaceDef::one = ' + eg.NameSpaceDef::one);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'eg.public::one'",
            "eg.public::onePub = Public one",
            'eg.public::onePub = ' + eg.public::onePub);
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'eg.one'",
            "eg.onePub = Public one",
            'eg.onePub = ' + eg.onePub);

Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'p.ns::one'",
            "p.NameSpaceDef::one = Namespace one",
            'p.NameSpaceDef::one = ' + p.NameSpaceDef::one);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'p.public::one'",
            "p.public::onePub = Public one",
            'p.public::onePub = ' + p.public::onePub);
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'p.one'",
            "p.onePub = Public one",
            'p.onePub = ' + p.onePub);

p.NameSpaceDef::one = "Namespace one again";

p.onePub = "Public one again";

// after setting value on p
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'eg.ns::one'",
            "eg.NameSpaceDef::one = Namespace one again",
            'eg.NameSpaceDef::one = ' + eg.NameSpaceDef::one);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'eg.public::one'",
            "eg.public::onePub = Public one again",
            'eg.public::onePub = ' + eg.public::onePub);
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'eg.one'",
            "eg.onePub = Public one again",
            'eg.onePub = ' + eg.onePub);

Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'p.ns::one'",
            "p.NameSpaceDef::one = Namespace one again",
            'p.NameSpaceDef::one = ' + p.NameSpaceDef::one);
Assert.expectEq("Test myProxy.getter of a variable in a namespace with dot operator -- 'p.public::one'",
            "p.public::onePub = Public one again",
            'p.public::onePub = ' + p.public::onePub);
Assert.expectEq("Test myObject.getter of a variable in a namespace with dot operator -- 'p.one'",
            "p.onePub = Public one again",
            'p.onePub = ' + p.onePub);

