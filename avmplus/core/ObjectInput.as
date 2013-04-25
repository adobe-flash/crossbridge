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
 * When an object being deserialized has implemented the IExternalizable Interface,
 * native code in the player will construct an ObjectInput to use as the parameter to:
 *      function readExternal(output:IDataOutput):void;
 *
 * ObjectInput's native code will redirect each of the below methods to
 * the same method on the real object being read from.
 */
    
    
[native(cls="ObjectInputClass", gc="exact", instance="ObjectInputObject", methods="auto")]
internal class ObjectInput implements IDataInput
{
    public native function readBytes(bytes:ByteArray, offset:uint=0, length:uint=0):void;
    public native function readBoolean():Boolean;
    public native function readByte():int;
    public native function readUnsignedByte():uint;
    public native function readShort():int;
    public native function readUnsignedShort():uint;
    public native function readInt():int;
    public native function readUnsignedInt():uint;
    public native function readFloat():Number;
    public native function readDouble():Number;
    public native function readMultiByte(length:uint, charSet:String):String;
    public native function readUTF():String;
    public native function readUTFBytes(length:uint):String;
    public native function get bytesAvailable():uint;
    public native function readObject():*;

    public native function get objectEncoding():uint;
    public native function set objectEncoding(version:uint):void;

    public native function get endian():String;
    public native function set endian(type:String):void;
}

}
