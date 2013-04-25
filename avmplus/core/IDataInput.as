/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.utils
{
    // Need to include this because IDataInput.as is not included into builtin.as.
    include "api-versions.as";

/**
* The IDataInput interface provides a set of methods for reading binary data.
* This interface is the I/O counterpart to the IDataOutput interface, which
* writes binary data.
* <p>All IDataInput and IDataOutput operations are "bigEndian" by default (the most significant
* byte in the sequence is stored at the lowest or first storage address),
* and are nonblocking.
* If insufficient data is available, an <code>EOFError</code> exception
* is thrown. Use the <code>IDataInput.bytesAvailable</code> property to determine
* how much data is available to read.</p>
*
* <p>Sign extension matters only when you read data, not when you write it. Therefore you do not need separate
* write methods to work with <code>IDataInput.readUnsignedByte()</code> and
* <code>IDataInput.readUnsignedShort()</code>. In other words:</p>
* <ul><li>Use <code>IDataOutput.writeByte()</code> with <code>IDataInput.readUnsignedByte()</code> and
* <code>IDataInput.readByte()</code>.</li>
* <li>Use <code>IDataOutput.writeShort()</code> with <code>IDataInput.readUnsignedShort()</code> and
* <code>IDataInput.readShort()</code>.</li></ul>
*
* @includeExample examples\DataInputExample.as -noswf
*
* @see flash.utils.IDataOutput IDataOutput interface
* @see #endian
* @see flash.filesystem.FileStream FileStream class
* @see flash.net.Socket Socket class
* @see flash.net.URLStream URLStream class
* @see flash.utils.ByteArray ByteArray class
* @see flash.errors.EOFError EOFError class
* @playerversion Flash 9
* @langversion 3.0
*
*  @playerversion Lite 4
*/
public interface IDataInput
{
    /**
     * Reads the number of data bytes, specified by the <code>length</code> parameter,
     * from the file stream, byte stream, or byte array. The bytes are read into the
     * ByteArray objected specified by the <code>bytes</code> parameter, starting at
     * the position specified by <code>offset</code>.
     * @param bytes The <code>ByteArray</code> object to read
     *              data into.
     * @param offset The offset into the <code>bytes</code> parameter at which data
     *               read should begin.
     * @param length The number of bytes to read.  The default value
     *               of 0 causes all available data to be read.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readBytes(bytes:ByteArray,
                              offset:uint=0,
                              length:uint=0):void;

    /**
     * Reads a Boolean value from the file stream, byte stream, or byte array. A single byte is read
     * and <code>true</code> is returned if the byte is nonzero,
     * <code>false</code> otherwise.
     * @return A Boolean value, <code>true</code> if the byte is nonzero,
     * <code>false</code> otherwise.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readBoolean():Boolean;

    /**
     * Reads a signed byte from the file stream, byte stream, or byte array.
     * @return The returned value is in the range -128 to 127.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readByte():int;

    /**
     * Reads an unsigned byte from the file stream, byte stream, or byte array.
     * @return The returned value is in the range 0 to 255.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readUnsignedByte():uint;

    /**
     * Reads a signed 16-bit integer from the file stream, byte stream, or byte array.
     * @return The returned value is in the range -32768 to 32767.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readShort():int;


    /**
     * Reads an unsigned 16-bit integer from the file stream, byte stream, or byte array.
     * @return The returned value is in the range 0 to 65535.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readUnsignedShort():uint;


    /**
     * Reads a signed 32-bit integer from the file stream, byte stream, or byte array.
     * @return The returned value is in the range -2147483648 to 2147483647.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readInt():int;
    
    /**
     * Reads an unsigned 32-bit integer from the file stream, byte stream, or byte array.
     * @return The returned value is in the range 0 to 4294967295.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readUnsignedInt():uint;


    /**
     * Reads an IEEE 754 single-precision floating point number from the file stream, byte stream, or byte array.
     * @return An IEEE 754 single-precision floating point number.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readFloat():Number;


    /**
     * Reads an IEEE 754 double-precision floating point number from the file stream, byte stream, or byte array.
     * @return An IEEE 754 double-precision floating point number.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readDouble():Number;


    /**
     * Reads a multibyte string of specified length from the file stream, byte stream, or byte array using the
     * specified character set.
     *
     *
     * @param length The number of bytes from the byte stream to read.
     * @param charSet The string denoting the character set to use to interpret the bytes.
     * Possible character set strings include <code>"shift-jis"</code>, <code>"cn-gb"</code>,
     * <code>"iso-8859-1"</code>, and others.
     * For a complete list, see <a href="../../charset-codes.html">Supported Character Sets</a>.
     *
     * <p><strong>Note:</strong> If the value for the <code>charSet</code> parameter is not recognized by the current
     * system, then <span platform="actionscript">Adobe<sup>&#xAE;</sup> Flash<sup>&#xAE;</sup> Player or</span>
     * Adobe<sup>&#xAE;</sup> AIR<sup>&#xAE;</sup> uses the system's default
     * code page as the character set. For example, a value for the <code>charSet</code> parameter, as in
     * <code>myTest.readMultiByte(22, "iso-8859-01")</code>, that uses  <code>01</code> instead of
     * <code>1</code> might work on your development system, but not on another system. On the other
     * system, <span platform="actionscript">Flash Player or</span> the AIR runtime will use the system's
     * default code page.</p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return UTF-8 encoded string.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword IDataInput, IDataInput.readMultiByte, readMultiByte
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readMultiByte(length:uint, charSet:String):String;


    /**
     * Reads a UTF-8 string from the file stream, byte stream, or byte array.  The string
     * is assumed to be prefixed with an unsigned short indicating
     * the length in bytes.
     *
     * <p>This method is similar to the <code>readUTF()</code>
     * method in the Java<sup>&#xAE;</sup> IDataInput interface.</p>
     * @return A UTF-8 string produced by the byte representation of characters.
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readUTF():String;
    
    
    /**
     * Reads a sequence of UTF-8 bytes from the byte stream or byte array and returns a string.
     * @param length The number of bytes to read.
     * @return A UTF-8 string produced by the byte representation of characters of the specified length.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readUTFBytes(length:uint):String;



    /**
     * Returns the number of bytes of data available for reading
     * in the input buffer.
     * User code must call <code>bytesAvailable</code> to ensure
     * that sufficient data is available before trying to read
     * it with one of the read methods.
     * @return An unsigned integer indicating the number of bytes available.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function get bytesAvailable():uint;


    /**
     * Reads an object from the file stream, byte stream, or byte array, encoded in AMF
     * serialized format.
     * @return The deserialized object
     *
     * @see #objectEncoding
     * @see ../../flash/net/package.html#registerClassAlias() flash.net.registerClassAlias()
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function readObject():*;


    /**
     * Used to determine whether the AMF3 or AMF0 format is used when writing or reading binary data using the
     * <code>readObject()</code> method. The value is a constant from the ObjectEncoding class.
     *
     * @see #readObject()
     * @see IDataOutput#writeObject()
     * @see flash.net.ObjectEncoding ObjectEncoding class
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function get objectEncoding():uint;
    [cppcall]
    function set objectEncoding(version:uint):void;


    /**
     * The byte order for the data, either the <code>BIG_ENDIAN</code> or <code>LITTLE_ENDIAN</code> constant
     * from the Endian class.
     *
     * @see flash.utils.Endian Endian class
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    [cppcall]
    function get endian():String;
    [cppcall]
    function set endian(type:String):void;
}

/**
 * Like IDataInput but provides readFloat4() additionally.
 */
[API(CONFIG::SWF_16)]
CONFIG::VMCFG_FLOAT
public interface IDataInput2 extends IDataInput
{
    /**
     * Reads a quadruple of IEEE 754 single-precision (32-bit) floating-point numbers from the byte stream.
     *
     * @throws EOFError There is not sufficient data available to read.
     * @return  A quadruple of single-precision (32-bit) floating-point numbers in the form of a float4.
     *
     * @playerversion Flash Player Cyril
     * @langversion 3.0
     */
    [cppcall]
    function readFloat4(): float4;
}

/*
 * [ggrossman 03/24/05] API SCRUB
 * - Created
 */

}
