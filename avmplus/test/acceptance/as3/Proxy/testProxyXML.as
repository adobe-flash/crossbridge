/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Proxy";
// var VERSION = "AS3";
// var TITLE   = "Proxy smoke tests ported from ATS: ATS9AS3 Automated/Proxy/testProxyXML";


var correct0:XMLList = new XMLList();
correct0 += new XML("value1");
correct0 += new XML("value2");

var correct4:XML =
<bravo attr1="value1" ns:attr1="value3" xmlns:ns="http://someuri">
<charlie attr1="value2" ns:attr1="value4"/>
</bravo>

var x1:XML =
<MLB>
<NationalLeague>
<Team mascot='Giants' city='San Francisco'/>
<Team mascot='Astros' city='Houston'/>
</NationalLeague>
<AmericanLeague>
<Team mascot='Athletics' city='Oakland'/>
<Team mascot='Rangers' city='Arlington'/>
</AmericanLeague>
</MLB>
                     

var x2:XML =
<alpha>
<bravo attr1="value1" ns:attr1="value3" xmlns:ns="http://someuri">
<charlie attr1="value2" ns:attr1="value4"/>
</bravo>
</alpha>

var esmith:XMLProxy = new XMLProxy(x2);

Assert.expectEq("Test calling isAttribute from getProperty with valid attribute",
            "isAttribute=true -- " + correct0, esmith.@attr1);
Assert.expectEq("Test calling isAttribute from getProperty with non-existing attribute",
            "isAttribute=true -- noExist does not exist", esmith.@noExist);

Assert.expectEq("Test calling isAttribute from getDescendants with valid attribute",
            correct0.toString(), esmith..@attr1.toString());
Assert.expectEq("Test calling isAttribute from getDescendants with non-existing attribute",
            "descendants is undefined for noExist", esmith..@noExist);

Assert.expectEq("Test calling getDescendants for existing element",
            correct4.toString(), esmith..bravo.toString());
Assert.expectEq("Test calling getDescendants for non-existing element",
            "descendants is undefined for noExist", esmith..noExist);

Assert.expectEq("Test calling callProperty with descendants", correct4.toString(),
            esmith.descendants('bravo').toString());
Assert.expectEq("Test calling callProperty with attributes", correct0.toString(),
            esmith.attributes('attr1').toString());

var qqq:XML =
<yoyo>
<abc a="10" b="20" c="30" d="40"/>
<def fff="567" ggg="888"/>
<dodo lll="10" ttt="555"/>
</yoyo>

var qqqProxy:XMLProxy = new XMLProxy(qqq);

// test delete of descendants

// delete existing element on proxy
var proxyDeleteStatus:Boolean = delete qqqProxy.dodo;

// delete non-existing element on proxy
var proxyDeleteStatus2:Boolean = delete qqqProxy.nonExist;

var correct10:XML =
<yoyo>
<abc a="10" b="20" c="30" d="40"/>
<def fff="567" ggg="888"/>
</yoyo>

Assert.expectEq("Test deleteProperty with descendants with existing element",
            true, proxyDeleteStatus);
Assert.expectEq("Test deleteProperty with descendants with non-existing element",
            true, proxyDeleteStatus2);

Assert.expectEq("Test getProperty on Proxy after deleting descendant on Proxy",
            correct10.toXMLString(), qqqProxy.toXMLString());
Assert.expectEq("Test getProperty on original XML after deleting descendant on Proxy",
            correct10.toXMLString(), qqq.toXMLString());

// delete and access was asynchronous, so I had to create a new xml for the second delete

var qqq2:XML =
<yoyo>
<abc a="10" b="20" c="30" d="40"/>
<def fff="567" ggg="888"/>
<dodo lll="10" ttt="555"/>
</yoyo>

var qqqProxy2:XMLProxy = new XMLProxy(qqq2);

// delete on original xml
var origDeleteStatus:Boolean = delete qqq2.def;

var correct12:XML =
<yoyo>
<abc a="10" b="20" c="30" d="40"/>
<dodo lll="10" ttt="555"/>
</yoyo>

Assert.expectEq("Test getProperty on Proxy after deleting descendant on original XML",
            correct12.toXMLString(), qqqProxy2.toXMLString());
Assert.expectEq("Test getProperty on original XML after deleting descendant on original XML",
            correct12.toXMLString(), qqq2.toXMLString());

var ppp:XML =
<abc a="10" b="20" c="30" d="40"/>

var pppProxy:XMLProxy = new XMLProxy(ppp);

// delete existing attribute on proxy
var proxyDeleteAttrStatus:Boolean = delete pppProxy.@b;

// delete non-existing attribute on proxy
var proxyDeleteAttrStatus2:Boolean = delete pppProxy.@nonExist;

Assert.expectEq("Test deleteProperty with attributes with existing element",
            true, proxyDeleteAttrStatus);
Assert.expectEq("Test deleteProperty with attributes with non-existing element",
            true, proxyDeleteAttrStatus2);

var correct16:XML =
<abc a="10" c="30" d="40"/>

Assert.expectEq("Test getProperty on Proxy after deleting attribute on Proxy",
            correct16.toXMLString(), pppProxy.toXMLString());
Assert.expectEq("Test getProperty on original XML after deleting attribute on Proxy",
            correct16.toXMLString(), ppp.toXMLString());

// delete attribute on original xml
var ppp2:XML =
<abc a="10" b="20" c="30" d="40"/>

var pppProxy2:XMLProxy = new XMLProxy(ppp2);

// delete on original xml
var origDeleteAttribStatus:Boolean = delete ppp2.@c;

var correct18:XML =
<abc a="10" b="20" d="40"/>

Assert.expectEq("Test getProperty on Proxy after deleting attribute on original XML",
            correct18.toXMLString(), pppProxy2.toXMLString());
Assert.expectEq("Test getProperty on original XML after deleting attribute on original XML",
            correct18.toXMLString(), ppp2.toXMLString());

// basic test case for QName with attributes and descendants
var xQ2:XML =
<alpha>
  <alpha2 xmlns="http://someuriNode/Default" xmlns:ns="http://someuriNode/NonDefault">
    <bravo attr1="value1" attr2="value11" ns:attr1="value3">
        <charlie attr1="value2" ns:attr1="value4" ns:attr2="value44"/>
        <ns:album>
            <ns:cd artist="Wilco" title="YankeeHotelFoxtrot"/>
        </ns:album>
    </bravo>
  </alpha2>
</alpha>

var x2Proxy:XMLProxy = new XMLProxy(xQ2);

nXML2Proxy = new Namespace("http://someuriNode/NonDefault");

qXML2ProxyAttr = new QName(nXML2Proxy, "attr1");
qXML2ProxyDesc = new QName(nXML2Proxy, "album");


var correct20:XMLList = new XMLList();
correct20 += new XML("value3");
correct20 += new XML("value4");

var correct21:XML =
<ns:album xmlns="http://someuriNode/Default" xmlns:ns="http://someuriNode/NonDefault">
  <ns:cd artist="Wilco" title="YankeeHotelFoxtrot"/>
</ns:album>

Assert.expectEq("Test calling getProperty for attribute with QName",
            "isAttribute=true -- " + correct20,
            x2Proxy.@[qXML2ProxyAttr]);
Assert.expectEq("Test calling getProperty for descendant with QName",
            "isAttribute=false -- "+correct21,
            x2Proxy[qXML2ProxyDesc]);


