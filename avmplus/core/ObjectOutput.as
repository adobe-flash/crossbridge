/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.utils
{
    
/*
 * This class is internal to flash.utils, so it does not require ASDoc's
 *
 * When an object being serialized has implemented the IExternalizable Interface,
 * native code in the player will construct an ObjectOutput to use as the parameter to:
 *      function writeExternal(output:IDataOutput):void;
 *
 * ObjectOutput's native code will redirect each of the below methods to
 * the same method on the real object being written to.
 */
    
[native(cls="ObjectOutputClass", gc="exact", instance="ObjectOutputObject", methods="auto")]
internal class ObjectOutput implements IDataOutput
{
    public native function writeBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void;
    public native function writeBoolean(value:Boolean):void;
    public native function writeByte(value:int):void;
    public native function writeShort(value:int):void;
    public native function writeInt(value:int):void;
    public native function writeUnsignedInt(value:uint):void;
    public native function writeFloat(value:Number):void;
    public native function writeDouble(value:Number):void;
    public native function writeMultiByte(value:String, charSet:String):void;
    public native function writeUTF(value:String):void;
    public native function writeUTFBytes(value:String):void;
    public native function writeObject(object:*):void;

    public native function get objectEncoding():uint;
    public native function set objectEncoding(version:uint):void;

    public native function get endian():String;
    public native function set endian(type:String):void;
}


}
