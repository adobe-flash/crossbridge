/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package {

    import avmplus.*;
import com.adobe.test.Assert;
    
    public namespace ns = "some_ns"
    public namespace ns2 = "another_ns"
    
    public interface IBar
    {
        function i0():*;
    };

    public interface IFoo
    {
        function i1():*;
        function get i2():*;
    };

    public class FooBase extends Object implements IBar
    {
        public function i0():* {}
    }

    public class Foo extends FooBase implements IFoo
    {
        public var z:int;
        
        [metahere(foo="vectorriffic")]
        public var vec:Vector.<int>;

        [metahere(foo="vectorriffic_foo")]
        public var vecfoo:Vector.<IFoo>;

        [metahere(foo="Foo get w")]
        public function get w():int { return 0; }
        
        function Foo(q:int, s:String = "") { z = q; }
        
        [metahere(foo="blah")]
        public function yah(d:Number):String { return String(z+d); }

        ns function yah(d:Number):String { return String(z+d); }

        public function i1():* {}
        public function get i2():* {}
    }

    [classmeta(a="b", c="d")]
    public class Foo2 extends Foo implements IFoo
    {
        public var aa:int;
        public const bb:IFoo = null;
        ns2 const bb:IFoo = undefined;
        
        function Foo2(q:int) { super(q); }
        override public function yah(d:Number):String { return String(z+d+1); }

        public function whatever(a:*, b:int, c:String):* {}
        ns2 function whatever(a:*, b:int, c:String = "hey"):* {}

        [metahere]
        override public function get w():int { return 1; }

        [metahere(foo="Foo2 set w")]
        public function set w(i:int):* {  }

        ns function get w():int { return 1; }

        public static function statfunc():void {}
    }

    var oi = describeType(new Foo2(2), FLASH10_FLAGS);
    var oc = describeType(Foo2, FLASH10_FLAGS);
    var ov = describeType(new Vector.<String>(), FLASH10_FLAGS);
    var oi2 = describeType(IBar, FLASH10_FLAGS);

    var expected = []
    var xc =
<type name="Foo2" base="Class" isDynamic="true" isFinal="true" isStatic="true">
    <extendsClass type="Class"/>
    <extendsClass type="Object"/>
    <method name="statfunc" declaredBy="Foo2" returnType="void"/>
    <accessor name="prototype" access="readonly" type="*" declaredBy="Class"/>
    <factory type="Foo2">
        <metadata name="classmeta">
            <arg key="a" value="b"/>
            <arg key="c" value="d"/>
        </metadata>
        <extendsClass type="Foo"/>
        <extendsClass type="FooBase"/>
        <extendsClass type="Object"/>
        <implementsInterface type="IFoo"/>
        <implementsInterface type="IBar"/>
        <constructor>
            <parameter index="1" type="int" optional="false"/>
        </constructor>
        <constant name="bb" type="IFoo"/>
        <method name="yah" declaredBy="Foo2" returnType="String">
            <parameter index="1" type="Number" optional="false"/>
            <metadata name="metahere">
                <arg key="foo" value="blah"/>
            </metadata>
        </method>
        <method name="whatever" declaredBy="Foo2" returnType="*">
            <parameter index="1" type="*" optional="false"/>
            <parameter index="2" type="int" optional="false"/>
            <parameter index="3" type="String" optional="false"/>
        </method>
        <accessor name="w" access="readwrite" type="int" declaredBy="Foo2">
            <metadata name="metahere"/>
            <metadata name="metahere">
                <arg key="foo" value="Foo2 set w"/>
            </metadata>
        </accessor>
        <constant name="bb" type="IFoo" uri="another_ns"/>
        <method name="whatever" declaredBy="Foo2" returnType="*" uri="another_ns">
            <parameter index="1" type="*" optional="false"/>
            <parameter index="2" type="int" optional="false"/>
            <parameter index="3" type="String" optional="true"/>
        </method>
        <variable name="aa" type="int"/>
        <variable name="z" type="int"/>
        <accessor name="i2" access="readonly" type="*" declaredBy="Foo"/>
        <method name="i0" declaredBy="FooBase" returnType="*"/>
        <method name="i1" declaredBy="Foo" returnType="*"/>
        <variable name="vecfoo" type="__AS3__.vec::Vector.&lt;IFoo&gt;">
            <metadata name="metahere">
                <arg key="foo" value="vectorriffic_foo"/>
            </metadata>
        </variable>
        <variable name="vec" type="__AS3__.vec::Vector.&lt;int&gt;">
            <metadata name="metahere">
                <arg key="foo" value="vectorriffic"/>
            </metadata>
        </variable>
    </factory>
</type>

    var xi =
<type name="Foo2" base="Foo" isDynamic="false" isFinal="false" isStatic="false">
    <metadata name="classmeta">
        <arg key="a" value="b"/>
        <arg key="c" value="d"/>
    </metadata>
    <extendsClass type="Foo"/>
    <extendsClass type="FooBase"/>
    <extendsClass type="Object"/>
    <implementsInterface type="IFoo"/>
    <implementsInterface type="IBar"/>
    <constructor>
        <parameter index="1" type="int" optional="false"/>
    </constructor>
    <constant name="bb" type="IFoo"/>
    <method name="yah" declaredBy="Foo2" returnType="String">
        <parameter index="1" type="Number" optional="false"/>
        <metadata name="metahere">
            <arg key="foo" value="blah"/>
        </metadata>
    </method>
    <method name="whatever" declaredBy="Foo2" returnType="*">
        <parameter index="1" type="*" optional="false"/>
        <parameter index="2" type="int" optional="false"/>
        <parameter index="3" type="String" optional="false"/>
    </method>
    <accessor name="w" access="readwrite" type="int" declaredBy="Foo2">
        <metadata name="metahere"/>
        <metadata name="metahere">
            <arg key="foo" value="Foo2 set w"/>
        </metadata>
    </accessor>
    <constant name="bb" type="IFoo" uri="another_ns"/>
    <method name="whatever" declaredBy="Foo2" returnType="*" uri="another_ns">
        <parameter index="1" type="*" optional="false"/>
        <parameter index="2" type="int" optional="false"/>
        <parameter index="3" type="String" optional="true"/>
    </method>
    <variable name="aa" type="int"/>
    <variable name="z" type="int"/>
    <accessor name="i2" access="readonly" type="*" declaredBy="Foo"/>
    <method name="i0" declaredBy="FooBase" returnType="*"/>
    <method name="i1" declaredBy="Foo" returnType="*"/>
    <variable name="vecfoo" type="__AS3__.vec::Vector.&lt;IFoo&gt;">
        <metadata name="metahere">
            <arg key="foo" value="vectorriffic_foo"/>
        </metadata>
    </variable>
    <variable name="vec" type="__AS3__.vec::Vector.&lt;int&gt;">
        <metadata name="metahere">
            <arg key="foo" value="vectorriffic"/>
        </metadata>
    </variable>
</type>

    var xv =
<type name="__AS3__.vec::Vector.&lt;String&gt;" base="__AS3__.vec::Vector.&lt;*&gt;" isDynamic="true" isFinal="false" isStatic="false">
    <extendsClass type="__AS3__.vec::Vector.&lt;*&gt;"/>
    <extendsClass type="Object"/>
    <accessor name="fixed" access="readwrite" type="Boolean" declaredBy="__AS3__.vec::Vector.&lt;*&gt;"/>
    <accessor name="length" access="readwrite" type="uint" declaredBy="__AS3__.vec::Vector.&lt;*&gt;"/>
</type>

    var xi2 =
<type name="IBar" base="Class" isDynamic="true" isFinal="true" isStatic="true">
  <extendsClass type="Class"/>
  <extendsClass type="Object"/>
  <accessor name="prototype" access="readonly" type="*" declaredBy="Class"/>
  <factory type="IBar">
    <method name="i0" declaredBy="IBar" returnType="*" uri="IBar"/>
  </factory>
</type>
    
    function sortXMLAttrs(x:XML):XML
    {
        var a:Array = [];
        for each (var att:XML in x.@*)
        {
            a.push({name:att.name(), value:att.toString()});
        }
        a.sortOn('name');
        for each (var o:* in a)
            delete x['@'+o.name]
        for each (o in a)
            x['@'+o.name] = o.value;
        return x;
    }

    function sortXML(x:XML):XML
    {
        // sort attrs first to normalize toXMLString compares
        sortXMLAttrs(x);

        XML.ignoreComments = true;
        XML.ignoreProcessingInstructions = true;
        XML.ignoreWhitespace = true;
        XML.prettyIndent = 4;
        XML.prettyPrinting = true;

        var a:Array = [];
        for each (var node:XML in x.children())
        {
            a.push(sortXML(node).toXMLString());
        }
        a.sort();

        x.setChildren(new XMLList);
        for each (var s:String in a)
        {
            x.appendChild(sortXML(new XML(s)));
        }
        
        
        
        return x;
    }
    
    // save values to restore afterwards (for ATS)
    var ignoreComments = XML.ignoreComments ;
    var ignoreProcessingInstructions = XML.ignoreProcessingInstructions ;
    var ignoreWhitespace = XML.ignoreWhitespace ;
    var prettyIndent = XML.prettyIndent;
    var prettyPrinting = XML.prettyPrinting ;
    
    Assert.expectEq("If test fails in debug mode,run without debug mode to see if it passes : sortXML(oc)", String(sortXML(xc)), String(sortXML(oc)));
    // describeType doesn't return the children in a canonical order; sort for stability
    Assert.expectEq("If test fails in debug mode,run without debug mode to see if it passes : sortXML(oi)", String(sortXML(xi)), String(sortXML(oi)));  
    // describeType doesn't return the children in a canonical order; sort for stability
    Assert.expectEq("sortXML(ov)", String(sortXML(xv)), String(sortXML(ov)));   // describeType doesn't return the children in a canonical order; sort for stability
    Assert.expectEq("If test fails in debug mode,run without debug mode to see if it passes : sortXML(oi2)", String(sortXML(xi2)), String(sortXML(oi2)));  
    // describeType doesn't return the children in a canonical order; sort for stability
    Assert.expectEq("getQualifiedClassName(Object)", "Object", getQualifiedClassName(Object));
    Assert.expectEq("getQualifiedClassName(Foo)", "Foo", getQualifiedClassName(Foo));
    Assert.expectEq("getQualifiedClassName(Foo2)", "Foo2", getQualifiedClassName(Foo2));
    Assert.expectEq("getQualifiedClassName(new Object)", "Object", getQualifiedClassName(new Object));
    Assert.expectEq("getQualifiedClassName(new Foo(1))", "Foo", getQualifiedClassName(new Foo(1)));
    Assert.expectEq("getQualifiedClassName(new Foo2(1))", "Foo2", getQualifiedClassName(new Foo2(1)));
    Assert.expectEq("getQualifiedSuperclassName(Object)", null, getQualifiedSuperclassName(Object));
    Assert.expectEq("getQualifiedSuperclassName(Foo)", "FooBase", getQualifiedSuperclassName(Foo));
    Assert.expectEq("getQualifiedSuperclassName(Foo2)", "Foo", getQualifiedSuperclassName(Foo2));
    Assert.expectEq("getQualifiedSuperclassName(new Object)", null, getQualifiedSuperclassName(new Object));
    Assert.expectEq("getQualifiedSuperclassName(new Foo(1))", "FooBase", getQualifiedSuperclassName(new Foo(1)));
    Assert.expectEq("getQualifiedSuperclassName(new Foo2(1))", "Foo", getQualifiedSuperclassName(new Foo2(1)));
    Assert.expectEq("describeType(null, FLASH10_FLAGS).toXMLString()", '<type name="null" isDynamic="false" isFinal="true" isStatic="false"/>', describeType(null, FLASH10_FLAGS).toXMLString());
    Assert.expectEq("describeType(void 0, FLASH10_FLAGS).toXMLString()", '<type name="void" isDynamic="false" isFinal="true" isStatic="false"/>', describeType(void 0, FLASH10_FLAGS).toXMLString());

    // https://bugzilla.mozilla.org/show_bug.cgi?id=524810
    var untypedInt = 34;
    Assert.expectEq("var untypedInt = 34; getQualifiedClassName(untypedInt)", "int", getQualifiedClassName(untypedInt));
    
    var typedInt:int = 1;
    Assert.expectEq("var typedInt:int = 1; getQualifiedClassName(typedInt)", "int", getQualifiedClassName(typedInt));
    
    var typedNumberIntValue:Number = 4;
    Assert.expectEq("var typedNumberIntValue:Number = 4; getQualifiedClassName(typedNumberIntValue)", "int", getQualifiedClassName(typedNumberIntValue));
    
    var typedNumber:Number = 3.14159;
    Assert.expectEq("var typedNumber:Number = 3.14159; getQualifiedClassName(typedNumber)", "Number", getQualifiedClassName(typedNumber));
    
    var inf:Number = Infinity;
    Assert.expectEq("var inf:Number = Infinity; getQualifiedClassName(inf)", "Number", getQualifiedClassName(inf));
    
    var nan:Number = NaN;
    Assert.expectEq("var nan:Number = NaN; getQualifiedClassName(nan)", "Number", getQualifiedClassName(nan));
    
    var int_nan:int = NaN;
    Assert.expectEq("var int_nan:int = NaN; getQualifiedClassName(int_nan)", "int", getQualifiedClassName(int_nan));
    
    var obj:Object = 3;
    Assert.expectEq("var obj:Object = 3; getQualifiedClassName(obj)", "int", getQualifiedClassName(obj));
    
    var objNum:Object = 3.4;
    Assert.expectEq("var objNum:Object = 3.4; getQualifiedClassName(objNum)", "Number", getQualifiedClassName(objNum));
    
    var bool:Boolean = 0;
    Assert.expectEq("var bool:Boolean = 0; getQualifiedClassName(bool)", "Boolean", getQualifiedClassName(bool));
    
    var bool2:Boolean = 1;
    Assert.expectEq("var bool2:Boolean = 1; getQualifiedClassName(bool2)", "Boolean", getQualifiedClassName(bool2));
    

    
    
    // restore XML values (for ATS)
    XML.ignoreComments = ignoreComments;
    XML.ignoreProcessingInstructions = ignoreProcessingInstructions;
    XML.ignoreWhitespace = ignoreWhitespace;
    XML.prettyIndent = prettyIndent;
    XML.prettyPrinting = prettyPrinting;
}

