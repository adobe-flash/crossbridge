/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package avmplus
{
    use namespace AS3;
    // -------------- internal --------------

    [native("DescribeTypeClass::describeTypeJSON")]
    internal native function describeTypeJSON(o:*, flags:uint):Object;

    internal const extendsXml:XML = <extendsClass />;
    internal const implementsXml:XML = <implementsInterface />;
    internal const constructorXml:XML = <constructor />;
    internal const constantXml:XML = <constant />;
    internal const variableXml:XML = <variable />;
    internal const accessorXml:XML= <accessor />;
    internal const methodXml:XML = <method />;
    internal const parameterXml:XML = <parameter />;
    internal const metadataXml:XML = <metadata />;
    internal const argXml:XML = <arg />;
    internal const typeXml:XML = <type />;
    internal const factoryXml:XML = <factory />;


    internal function describeParams(x:XML, parameters:Object):void
    {
        var c:XMLList = x.*;
        for (var i in parameters)
        {
            var p = parameters[i];
            var f:XML = parameterXml.copy();
            f.@index = i+1;
            f.@type = p.type;
            f.@optional = p.optional;

            c[c.length()] = f;
        }
    }

    internal function describeMetadata(x:XML, metadata:Array):void
    {
        var c:XMLList = x.*;
        for each (var md in metadata)
        {
            var m:XML = metadataXml.copy();
            m.@name = md.name;
            for each (var i in md.value)
            {
                var a:XML = argXml.copy()
                a.@key = i.key;
                a.@value = i.value;

                m.AS3::appendChild(a);
            }
            c[c.length()] = m;
        }
    }

    internal function finish(e:XML, i:Object):void
    {
        if (i.uri !== null) e.@uri = i.uri;
        if (i.metadata !== null) describeMetadata(e, i.metadata);
    }

    internal function describeTraits(x:XML, traits:Object):void
    {
        var c:XMLList = x.*;

        for each (var i in traits.bases)
        {
            var base:String = i;

            var e:XML = extendsXml.copy();
            e.@type = base;

            c[c.length()] = e;
        }
        for each (var i in traits.interfaces)
        {
            var interf:String = i;

            var e:XML = implementsXml.copy();
            e.@type = interf;

            c[c.length()] = e;
        }
        if (traits.constructor !== null)
        {
            var e:XML = constructorXml.copy();
            describeParams(e, traits.constructor);
            c[c.length()] = e;
        }

        for each (var i in traits.variables)
        {
            var variable:Object = i;

            var e:XML = (variable.access == "readonly") ? constantXml.copy() : variableXml.copy();
            e.@name = variable.name;
            e.@type = variable.type;

            finish(e, variable);

            c[c.length()] = e;
        }
        for each (var i in traits.accessors)
        {
            var accessor:Object = i;

            var e:XML = accessorXml.copy();
            e.@name = accessor.name;
            e.@access = accessor.access;
            e.@type = accessor.type;
            e.@declaredBy = accessor.declaredBy;

            finish(e, accessor);

            c[c.length()] = e;
        }
        for each (var i in traits.methods)
        {
            var method:Object = i;

            var e:XML = methodXml.copy();
            e.@name = method.name;
            e.@declaredBy = method.declaredBy;
            e.@returnType = method.returnType;

            describeParams(e, method.parameters);
            finish(e, method);

            c[c.length()] = e;
        }
        describeMetadata(x, traits.metadata);
    }

    // -------------- public --------------

    // this bit replicates a bug in Flash9/10, where a method that uses a custom namespace
    // won't be in the output if any of its base classes (or interfaces) also define a method
    // in that custom namespace.
    public const HIDE_NSURI_METHODS:uint    = 0x0001;
    public const INCLUDE_BASES:uint         = 0x0002;
    public const INCLUDE_INTERFACES:uint    = 0x0004;
    public const INCLUDE_VARIABLES:uint     = 0x0008;
    public const INCLUDE_ACCESSORS:uint     = 0x0010;
    public const INCLUDE_METHODS:uint       = 0x0020;
    public const INCLUDE_METADATA:uint      = 0x0040;
    public const INCLUDE_CONSTRUCTOR:uint   = 0x0080;
    public const INCLUDE_TRAITS:uint        = 0x0100;
    public const USE_ITRAITS:uint           = 0x0200;
    // if set, hide everything from the base Object class
    public const HIDE_OBJECT:uint           = 0x0400;

    // set of flags that replicates the behavior of flash.util.describeType in FlashPlayer 9 & 10
    public const FLASH10_FLAGS:uint =   INCLUDE_BASES |
                                        INCLUDE_INTERFACES |
                                        INCLUDE_VARIABLES |
                                        INCLUDE_ACCESSORS |
                                        INCLUDE_METHODS |
                                        INCLUDE_METADATA |
                                        INCLUDE_CONSTRUCTOR |
                                        INCLUDE_TRAITS |
                                        // include this buggy behavior by default, to match legacy Flash behavior
                                        HIDE_NSURI_METHODS |
                                        // Flash hides everything in class Object
                                        HIDE_OBJECT;

    public function describeType(value:*, flags:uint):XML
    {
        var o:Object = describeTypeJSON(value, flags);
        var x:XML = typeXml.copy();
        x.@name = o.name;
        if (o.traits.bases.length)
            x.@base = o.traits.bases[0];
        x.@isDynamic = o.isDynamic;
        x.@isFinal = o.isFinal;
        x.@isStatic = o.isStatic;
        describeTraits(x, o.traits);

        var oi:Object = describeTypeJSON(value, flags | USE_ITRAITS);
        if (oi !== null)
        {
            var e:XML = factoryXml.copy();
            e.@type = oi.name;
            describeTraits(e, oi.traits);
            x.AS3::appendChild(e);
        }

        return x;
    }

    [native("DescribeTypeClass::getQualifiedClassName")]
    public native function getQualifiedClassName(value:*):String;

    [native("DescribeTypeClass::getQualifiedSuperclassName")]
    public native function getQualifiedSuperclassName(value:*):String;
}
