/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import avmplus.*;
import com.adobe.test.Assert;

function AddStrictTestCase(desc, expected, actual)
{
    if (typeof(expected)==typeof(actual))
        Assert.expectEq(desc, expected, actual);
    else
        Assert.expectEq(desc, typeof(expected)+"["+expected+"]","(STRICT EQ); "+typeof(actual)+"["+actual+"]");
}

var dummy_number = NaN;
var isAS3:Boolean = dummy_number.toString == dummy_number.AS3::toString;

function getFloat4Prop(name):String
{
   var string = '';
   for ( var prop in float4 )
   {
       string += ( prop == name ) ? prop : '';
   }
   return string;
}

function getFloat4ProtoProp(name):String
{
   var string = '';
   for ( var prop in float4.prototype )
   {
       string += ( prop == name ) ? prop : '';
   }
   return string;
}

function FloatRawBits(param:float):int
{
    // Don't catch exceptions here - caller function should catch them
    var byteArr:ByteArray = new ByteArray();
    byteArr.writeFloat(param);
    byteArr.position = 0;
    var result:int =  byteArr.readInt();
    return result;
}

function DoubleRawBits(param:Number):Array
{
    // Don't catch exceptions here - caller function should catch them
    var byteArr:ByteArray = new ByteArray();
    byteArr.writeDouble(param);
    byteArr.position = 0;
    var result0:int =  byteArr.readInt();
    var result1:int =  byteArr.readInt();
    return [result0,result1];
}

function MyValueAlteringObject(value)
{
    this.value = value;
    this.valueOf = function(){var v = ++this.value; return v};
}
function MyStringObject(value)
{
    this.value = value;
    this.toString = function(){return this.value+''};
}
function MyObject(value)
{
    this.value = value;
    this.valueOf = function(){return this.value};
}
