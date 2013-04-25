/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.utils
{

include "api-versions.as"

/**
 * The CompressionAlgorithm class defines string constants for the names of compress and uncompress options. These constants
 * are used as values of the <code>algorithm</code> parameter of the <code>ByteArray.compress()</code>
 * and <code>ByteArray.uncompress()</code> methods.
 *
 * @see flash.utils.ByteArray#compress()
 * @see flash.utils.ByteArray#uncompress()
 *
 * @playerversion Flash 10
 * @playerversion AIR 1.0
 * @langversion 3.0
 *
 */
public final class CompressionAlgorithm
{
    /**
     * Defines the string to use for the zlib compression algorithm.
     *
     * @playerversion Flash 10
     * @playerversion AIR 1.0
     * @langversion 3.0
     *
     */
    public static const ZLIB:String = "zlib";
    /**
     * Defines the string to use for the deflate compression algorithm.
     *
     * @playerversion Flash 10
     * @playerversion AIR 1.0
     * @langversion 3.0
     *
     */
    public static const DEFLATE:String = "deflate";
    /**
     * Defines the string to use for the lzma compression algorithm.
     *
     * @playerversion Flash 11.4
     * @playerversion AIR 3.4
     * @langversion 3.0
     *
     */
    public static const LZMA:String = "lzma";    
};

// Provide dummy definitions here of IDataInput2 and IDataOutput2, because they will
// not be provided by IDataInput.as and IDataOutput.as if VMCFG_FLOAT is disabled.

CONFIG const NO_VMCFG_FLOAT = !CONFIG::VMCFG_FLOAT;

CONFIG::NO_VMCFG_FLOAT
internal interface IDataInput2 extends IDataInput {
}

CONFIG::NO_VMCFG_FLOAT
internal interface IDataOutput2 extends IDataOutput {
}

//
// ByteArray
//


/**
 * The ByteArray class provides methods and properties to optimize reading, writing,
 * and working with binary data.
 *
 * <p><em>Note:</em> The ByteArray class is for advanced developers who need to access
 * data on the byte level.</p>
 *
 * <p>In-memory data is a packed array (the most compact representation for the data type)
 * of bytes, but an instance of the ByteArray
 * class can be manipulated with the standard <code>[]</code> (array access) operators.
 * It also can be read and written to as an in-memory file, using
 * methods similar to those in the URLStream and Socket classes.</p>
 *
 * <p>In addition, zlib and lzma compression and decompression are supported, as
 * well as Action Message Format (AMF) object serialization.</p>
 *
 * <p>Possible uses of the ByteArray class include the following:
 *
 * <ul>
 *
 *   <li>Creating a custom protocol to connect to a server.</li>
 *
 *   <li>Writing your own URLEncoder/URLDecoder.</li>
 *
 *   <li platform="actionscript">Writing your own AMF/Remoting packet.</li>
 *
 *   <li>Optimizing the size of your data by using data types.</li>
 *
 *   <li>Working with binary data loaded from a file in
 *      Adobe<sup>&#xAE;</sup> AIR<sup>&#xAE;</sup>.</li>
 *
 * </ul>
 * </p>
 *
 * @includeExample examples\ByteArrayExample.as -noswf
 * @see ../../operators.html#array_access [] (array access)
 * @see flash.net.Socket Socket class
 * @see flash.net.URLStream URLStream class
 * @playerversion Flash 9
 * @langversion 3.0
 * @helpid
 * @refpath
 * @keyword ByteArray
 *
 * @playerversion Lite 4
 */

[native(cls="ByteArrayClass", gc="exact", instance="ByteArrayObject", methods="auto")]
public class ByteArray implements IDataInput2, IDataOutput2
{

    /**
     * Creates a ByteArray instance representing a packed array of bytes, so that you can use the methods and properties in this class to optimize your data storage and stream.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     * @playerversion Lite 4
     */
    public function ByteArray(){}

    /**
     * Reads the number of data bytes, specified by the <code>length</code> parameter, from the byte stream.
     * The bytes are read into the ByteArray object specified by the <code>bytes</code> parameter,
     * and the bytes are written into the destination ByteArray starting at the position specified by <code>offset</code>.
     *
     * @param bytes The ByteArray object to read data into.
     * @param offset The offset (position) in <code>bytes</code> at which the read data should be written.
     * @param length The number of bytes to read.  The default value of 0 causes all available data to be read.
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @throws RangeError The value of the supplied offset and length, combined, is greater than the maximum for a uint.
     * @internal does this throw IOError An I/O error occurred on the byte stream,
     * or the byte stream is not open?
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readBytes, readBytes
     *
     * @playerversion Lite 4
     */
    public native function readBytes(bytes:ByteArray,
                                     offset:uint=0,
                                     length:uint=0):void;


    /**
     * Writes a sequence of <code>length</code> bytes from the
     * specified byte array, <code>bytes</code>,
     * starting <code>offset</code>(zero-based index) bytes
     * into the byte stream.
     *
     * <p>If the <code>length</code> parameter is omitted, the default
     * length of 0 is used; the method writes the entire buffer starting at
     * <code>offset</code>.
     * If the <code>offset</code> parameter is also omitted, the entire buffer is
     * written. </p> <p>If <code>offset</code> or <code>length</code>
     * is out of range, they are clamped to the beginning and end
     * of the <code>bytes</code> array.</p>
     *
     * @param ByteArray The ByteArray object.
     * @param offset A zero-based index indicating the position into the array to begin writing.
     * @param length An unsigned integer indicating how far into the buffer to write.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeBytes, writeBytes
     *
     * @playerversion Lite 4
     */
    public native function writeBytes(bytes:ByteArray,
                                      offset:uint=0,
                                      length:uint=0):void;

    /**
     * Writes a Boolean value. A single byte is written according to the <code>value</code> parameter,
     * either 1 if <code>true</code> or 0 if <code>false</code>.
     *
     * @param value A Boolean value determining which byte is written. If the parameter is <code>true</code>,
     * the method writes a 1; if <code>false</code>, the method writes a 0.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeBoolean, writeBoolean
     *
     * @playerversion Lite 4
     */
    public native function writeBoolean(value:Boolean):void;

    /**
     * Writes a byte to the byte stream.
     * <p>The low 8 bits of the
     * parameter are used. The high 24 bits are ignored. </p>
     *
     * @param value A 32-bit integer. The low 8 bits are written to the byte stream.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeByte, writeByte
     *
     * @playerversion Lite 4
     */
    public native function writeByte(value:int):void;

    /**
     * Writes a 16-bit integer to the byte stream. The low 16 bits of the parameter are used.
     * The high 16 bits are ignored.
     *
     * @param A 32-bit integer, whose low 16 bits are written to the byte stream.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeShort, writeShort
     *
     * @playerversion Lite 4
     */
    public native function writeShort(value:int):void;

    /**
     * Writes a 32-bit signed integer to the byte stream.
     *
     * @param value An integer to write to the byte stream.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeInt, writeInt
     *
     * @playerversion Lite 4
     */
    public native function writeInt(value:int):void;

    /**
     * Writes a 32-bit unsigned integer to the byte stream.
     *
     * @param value An unsigned integer to write to the byte stream.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeUnsignedInt, writeUnsignedInt
     *
     * @playerversion Lite 4
     */
    public native function writeUnsignedInt(value:uint):void;

    /**
     * Writes an IEEE 754 single-precision (32-bit) floating-point number to the byte stream.
     *
     * @param Number A single-precision (32-bit) floating-point number.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeFloat, writeFloat
     *
     * @playerversion Lite 4
     */
    public native function writeFloat(value:Number):void;

    /**
     * Writes a quadruple of IEEE 754 single-precision (32-bit) floating-point numbers to the byte stream.
     *
     * @param value  A float4 datum: a quadruple of single-precision (32-bit) floating-point numbers.
     * @playerversion Flash Player Cyril
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeFloat4, writeFloat4
     */
    [API(CONFIG::SWF_16)]
    CONFIG::VMCFG_FLOAT
    public native function writeFloat4(value:float4):void;

    /**
     * Writes an IEEE 754 double-precision (64-bit) floating-point number to the byte stream.
     *
     * @param Number A double-precision (64-bit) floating-point number.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeDouble, writeDouble
     *
     * @playerversion Lite 4
     */
    public native function writeDouble(value:Number):void;

    /**
     * Writes a multibyte string to the byte stream using the specified character set.
     *
     * @param value The string value to be written.
     * @param charSet The string denoting the character set to use. Possible character set strings
     * include <code>"shift-jis"</code>, <code>"cn-gb"</code>, <code>"iso-8859-1"</code>, and others.
     * For a complete list, see <a href="../../charset-codes.html">Supported Character Sets</a>.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeMultiByte, writeMultiByte
     *
     * @playerversion Lite 4
     */
    public native function writeMultiByte(value:String, charSet:String):void;

    /**
     * Writes a UTF-8 string to the byte stream. The length of the UTF-8 string in bytes
     * is written first, as a 16-bit integer, followed by the bytes representing the
     * characters of the string.
     *
     * @param value The string value to be written.
     * @throws RangeError If the length is larger than
     * 65535.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeUTF, writeUTF
     *
     * @playerversion Lite 4
     */
    public native function writeUTF(value:String):void;

    /**
     * Writes a UTF-8 string to the byte stream. Similar to the <code>writeUTF()</code> method,
     * but <code>writeUTFBytes()</code> does not prefix the string with a 16-bit length word.
     *
     * @param value The string value to be written.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeUTFBytes, writeUTFBytes
     *
     * @playerversion Lite 4
     */
    public native function writeUTFBytes(value:String):void;

    /**
     * Reads a Boolean value from the byte stream. A single byte is read,
     * and <code>true</code> is returned if the byte is nonzero,
     * <code>false</code> otherwise.
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return Returns <code>true</code> if the byte is nonzero, <code>false</code> otherwise.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readBoolean, readBoolean
     *
     * @playerversion Lite 4
     */
    public native function readBoolean():Boolean;

    /**
     * Reads a signed byte from the byte stream.
     * <p>The returned value is in the range -128 to 127.</p>
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return An integer between -128 and 127.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readByte, readByte
     *
     * @playerversion Lite 4
     */
    public native function readByte():int;

    /**
     * Reads an unsigned byte from the byte stream.
     *
     * <p>The returned value is in the range 0 to 255. </p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A 32-bit unsigned integer between 0 and 255.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readUnsignedByte, readUnsignedByte
     *
     * @playerversion Lite 4
     */
    public native function readUnsignedByte():uint;

    /**
     * Reads a signed 16-bit integer from the byte stream.
     *
     * <p>The returned value is in the range -32768 to 32767.</p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A 16-bit signed integer between -32768 and 32767.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readShort, readShort
     *
     * @playerversion Lite 4
     */
    public native function readShort():int;


    /**
     * Reads an unsigned 16-bit integer from the byte stream.
     *
     * <p>The returned value is in the range 0 to 65535. </p>
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A 16-bit unsigned integer between 0 and 65535.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readUnsignedShort, readUnsignedShort
     *
     * @playerversion Lite 4
     */
    public native function readUnsignedShort():uint;

    /**
     * Reads a signed 32-bit integer from the byte stream.
     *
     * <p>The returned value is in the range -2147483648 to 2147483647.</p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A 32-bit signed integer between -2147483648 and 2147483647.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readInt, readInt
     *
     * @playerversion Lite 4
     */
    public native function readInt():int;

    /**
     * Reads an unsigned 32-bit integer from the byte stream.
     *
     * <p>The returned value is in the range 0 to 4294967295. </p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A 32-bit unsigned integer between 0 and 4294967295.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readUnsignedInt, readUnsignedInt
     *
     * @playerversion Lite 4
     */
    public native function readUnsignedInt():uint;

    /**
     * Reads an IEEE 754 single-precision (32-bit) floating-point number from the byte stream.
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return  A single-precision (32-bit) floating-point number.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readFloat, readFloat
     *
     * @playerversion Lite 4
     */
    public native function readFloat():Number;

    /**
     * Reads a quadruple of IEEE 754 single-precision (32-bit) floating-point numbers from the byte stream.
     *
     * @throws EOFError There is not sufficient data available to read.
     * @return  A quadruple of single-precision (32-bit) floating-point numbers in the form of a float4.
     *
     * @playerversion Flash Player Cyril
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readFloat4, readFloat4
     */
    [API(CONFIG::SWF_16)]
    CONFIG::VMCFG_FLOAT
    public native function readFloat4():float4;

    /**
     * Reads an IEEE 754 double-precision (64-bit) floating-point number from the byte stream.
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A double-precision (64-bit) floating-point number.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readDouble, readDouble
     *
     * @playerversion Lite 4
     */
    public native function readDouble():Number;

    /**
     * Reads a multibyte string of specified length from the byte stream using the
     * specified character set.
     *
     *
     * @param length The number of bytes from the byte stream to read.
     * @param charSet The string denoting the character set to use to interpret the bytes.
     * Possible character set strings include <code>"shift-jis"</code>, <code>"cn-gb"</code>,
     * <code>"iso-8859-1"</code>, and others.
     * For a complete list, see <a href="../../charset-codes.html">Supported Character Sets</a>.
     * <p><strong>Note:</strong> If the value for the <code>charSet</code> parameter
     * is not recognized by the current system, the application uses the system's default
     * code page as the character set. For example, a value for the <code>charSet</code> parameter,
     * as in <code>myTest.readMultiByte(22, "iso-8859-01")</code> that uses <code>01</code> instead of
     * <code>1</code> might work on your development system, but not on another system.
     * On the other system, the application will use the system's default code page.</p>
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return UTF-8 encoded string.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readMultiByte, readMultiByte
     *
     * @playerversion Lite 4
     */
    public native function readMultiByte(length:uint, charSet:String):String;

    /**
     * Reads a UTF-8 string from the byte stream.  The string
     * is assumed to be prefixed with an unsigned short indicating
     * the length in bytes.
     *
     *
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return UTF-8 encoded  string.
     * @playerversion Flash 9
     * @langversion 3.0
     * @see flash.utils.IDataInput#readUTF()
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readUTF, readUTF
     *
     * @playerversion Lite 4
     */
    public native function readUTF():String;

    /**
     * Reads a sequence of UTF-8 bytes specified by the <code>length</code>
     * parameter from the byte stream and returns a string.
     *
     * @param length An unsigned short indicating the length of the UTF-8 bytes.
     * @throws EOFError There is not sufficient data available
     * to read.
     * @return A string composed of the UTF-8 bytes of the specified length.
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readUTFBytes, readUTFBytes
     *
     * @playerversion Lite 4
     */
    public native function readUTFBytes(length:uint):String;

    /**
     * The length of the ByteArray object, in bytes.
     *
     * <p>If the length is set to a value that is larger than the current length,
     * the right side  of the byte array is filled with zeros.</p>
     *
     * <p>If the length is set to a value that is smaller than the
     * current length, the byte array is truncated.</p>
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.length, length
     *
     * @playerversion Lite 4
     */
    public native function get length():uint;
    public native function set length(value:uint):void;

    /**
     * Writes an object into the byte array in AMF
     * serialized format.
     *
     * @param object The object to serialize.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.writeObject, writeObject
     * @see ../../flash/net/package.html#registerClassAlias() flash.net.registerClassAlias()
     *
     *
     * @playerversion Lite 4
     */
    public native function writeObject(object:*):void;

    /**
     * Reads an object from the byte array, encoded in AMF
     * serialized format.
     *
     * @return The deserialized object.
     * @throws EOFError There is not sufficient data available
     * to read.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.readObject, readObject
     * @see ../../flash/net/package.html#registerClassAlias() flash.net.registerClassAlias()
     *
     *
     * @playerversion Lite 4
     */
    public native function readObject():*;

    [API(CONFIG::FP_10_0)]
    /**
     * Compresses the byte array using the deflate compression algorithm.
     * The entire byte array is compressed.
     *
     * <p>After the call, the <code>length</code> property of the ByteArray is set to the new length.
     * The <code>position</code> property is set to the end of the byte array.</p>
     *
     * <p>The deflate compression algorithm is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1951.txt">http://www.ietf.org/rfc/rfc1951.txt</a>.</p>
     *
     * <p>In order to use the deflate format to compress a ByteArray instance's
     * data in a specific format such as gzip or zip, you cannot simply call
     * <code>deflate()</code>. You must create a ByteArray structured
     * according to the compression format's specification, including the appropriate
     * metadata as well as the compressed data obtained using the deflate format.
     * Likewise, in order to decode data compressed in a format such
     * as gzip or zip, you can't simply call <code>inflate()</code>
     * on that data. First, you must separate the metadata from the compressed data, and you can
     * then use the deflate format to decompress the compressed data.</p>
     *
     * @see #inflate()
     * @playerversion Flash 10
     * @playerversion AIR 1.5
     * @langversion 3.0
     *
     * @playerversion Lite 4
     */
    public function deflate():void
    {
        _compress("deflate");
    }
    
    private native function _compress(algorithm:String):void;

    /**
     * Compresses the byte array. The entire byte array is compressed. For content
     * running in Adobe AIR, you can specify a compression algorithm by passing a
     * value (defined in the CompressionAlgorithm class) as the <code>algorithm</code>
     * parameter. <span platform="actionscript">Flash Player supports only the default
     * algorithm, zlib.</span>
     *
     * <p>After the call, the <code>length</code> property of the ByteArray is set to the new length.
     * The <code>position</code> property is set to the end of the byte array.</p>
     *
     * <p>The zlib compressed data format is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1950.txt">http://www.ietf.org/rfc/rfc1950.txt</a>.</p>
     *
     * <p>The deflate compression algorithm is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1951.txt">http://www.ietf.org/rfc/rfc1951.txt</a>.</p>
     *
     * <p>The lzma compression algorithm is described at
     * <a target="mm_external" href="http://www.7-zip.org/7z.html">http://www.7-zip.org/7z.html</a>.</p>
     *
     * <p>The deflate compression algorithm is used in several compression
     * formats, such as zlib, gzip, some zip implementations, and others. When data is
     * compressed using one of those compression formats, in addition to storing
     * the compressed version of the original data, the compression format data
     * (for example, the .zip file) includes metadata information. Some examples of
     * the types of metadata included in various file formats are file name,
     * file modification date/time, original file size, optional comments, checksum
     * data, and more.</p>
     *
     * <p>For example, when a ByteArray is compressed using the zlib algorithm,
     * the resulting ByteArray is structured in a specific format. Certain bytes contain
     * metadata about the compressed data, while other bytes contain the actual compressed
     * version of the original ByteArray data. As defined by the zlib compressed data
     * format specification, those bytes (that is, the portion containing
     * the compressed version of the original data) are compressed using the deflate
     * algorithm. Consequently those bytes are identical to the result of calling
     * <code>compress(<span class="javascript">air.</span>CompressionAlgorithm.DEFLATE)</code>
     * on the original ByteArray. However, the result from
     * <code>compress(<span class="javascript">air.</span>CompressionAlgorithm.ZLIB)</code> includes
     * the extra metadata, while the <code>compress(<span platform="javascript">air.</span>CompressionAlgorithm.DEFLATE)</code>
     * result includes only the compressed version of the original ByteArray data and nothing else.</p>
     *
     * <p>In order to use the deflate format to compress a ByteArray instance's
     * data in a specific format such as gzip or zip, you cannot simply call
     * <code>compress(<span platform="javascript">air.</span>CompressionAlgorithm.DEFLATE)</code>.
     * You must create a ByteArray structured
     * according to the compression format's specification, including the appropriate
     * metadata as well as the compressed data obtained using the deflate format.
     * Likewise, in order to decode data compressed in a format such
     * as gzip or zip, you can't simply call <code>uncompress(<span platform="javascript">air.</span>CompressionAlgorithm.DEFLATE)</code>
     * on that data. First, you must separate the metadata from the compressed data, and you can
     * then use the deflate format to decompress the compressed data.</p>
     *
     * @param algorithm The compression algorithm to use when compressing. Valid values are defined as
     * constants in the CompressionAlgorithm class. The default is to use zlib format.
     * <span platform="actionscript">This parameter is only recognized for content running in Adobe AIR.
     * Flash Player supports only the default algorithm, zlib, and throws an exception if you attempt to pass
     * a value for this parameter.</span> Calling <code>compress(<span platform="javascript">air.</span>CompressionAlgorithm.DEFLATE)</code>
     * has the same effect as calling the <code>deflate()</code> method.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     *
     * @see #uncompress()
     * @see flash.utils.CompressionAlgorithm
     * @keyword ByteArray, ByteArray.compress, compress
     *
     */
    public function compress(algorithm:String = CompressionAlgorithm.ZLIB):void
    {
        _compress(algorithm);
    }

    [API(CONFIG::FP_10_0)]
    /**
     * Decompresses the byte array using the deflate compression algorithm.
     * The byte array must have been compressed using the same algorithm.
     *
     * <p>After the call, the <code>length</code> property of the ByteArray is set to the new length.
     * The <code>position</code> property is set to 0.</p>
     *
     * <p>The deflate compression algorithm is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1951.txt">http://www.ietf.org/rfc/rfc1951.txt</a>.</p>
     *
     * <p>In order to decode data compressed in a format that uses the deflate compression algorithm,
     * such as data in gzip or zip format, it will not work to simply call <code>inflate()</code> on
     * a ByteArray containing the compression formation data. First, you must separate the metadata that is
     * included as part of the compressed data format from the actual compressed data. For more
     * information, see the <code>compress()</code> method description.</p>
     *
     * @throws IOError The data is not valid compressed data; it was not compressed with the
     * same compression algorithm used to compress.
     *
     * @see #deflate()
     *
     * @playerversion Flash 10
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.uncompress, uncompress
     *
     */
    public function inflate():void
    {
        _uncompress("deflate");
    }
    
    private native function _uncompress(algorithm:String):void;


    /**
     * Decompresses the byte array. For content running in Adobe AIR, you can specify
     * a compression algorithm by passing a value (defined in the CompressionAlgorithm class)
     * as the <code>algorithm</code> parameter. The byte array must have been compressed
     * using the same algorithm. <span platform="actionscript">Flash Player supports only the
     * default algorithm, zlib.</span>
     *
     * <p>After the call, the <code>length</code> property of the ByteArray is set to the new length.
     * The <code>position</code> property is set to 0.</p>
     *
     * <p>The zlib compressed data format is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1950.txt">http://www.ietf.org/rfc/rfc1950.txt</a>.</p>
     *
     * <p>The deflate compression algorithm is described at
     * <a target="mm_external" href="http://www.ietf.org/rfc/rfc1951.txt">http://www.ietf.org/rfc/rfc1951.txt</a>.</p>
     *
     * <p>In order to decode data compressed in a format that uses the deflate compression algorithm,
     * such as data in gzip or zip format, it will not work to call
     * <code>uncompress(CompressionAlgorithm.DEFLATE)</code> on
     * a ByteArray containing the compression formation data. First, you must separate the metadata that is
     * included as part of the compressed data format from the actual compressed data. For more
     * information, see the <code>compress()</code> method description.</p>
     *
     * @param algorithm The compression algorithm to use when decompressing. This must be the
     * same compression algorithm used to compress the data. Valid values are defined as
     * constants in the CompressionAlgorithm class. The default is to use zlib format. This parameter
     * is only recognized for content running in Adobe AIR. <span platform="actionscript">Flash Player
     * supports only the default algorithm, zlib, and throws an exception if you attempt to pass
     * a value for this parameter.</span>
     *
     * @throws IOError The data is not valid compressed data; it was not compressed with the
     * same compression algorithm used to compress.
     *
     * @see #compress()
     * @see flash.utils.CompressionAlgorithm
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.uncompress, uncompress
     *
     * @playerversion Lite 4
     */
    public function uncompress(algorithm:String = CompressionAlgorithm.ZLIB):void
    {
        _uncompress(algorithm);
    }

    /**
     * Converts the byte array to a string.
     * If the data in the array begins with a Unicode byte order mark, the application will honor that mark
     * when converting to a string. If <code>System.useCodePage</code> is set to <code>true</code>, the
     * application will treat the data in the array as being in the current system code page when converting.
     *
     * @return The string representation of the byte array.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.toString, toString
     *
     * @playerversion Lite 4
     */
    public function toString():String { return _toString(); }
    private native function _toString():String;

    /**
     * The number of bytes of data available for reading
     * from the current position in the byte array to the
     * end of the array.
     *
     * <p>Use the <code>bytesAvailable</code> property in conjunction
     * with the read methods each time you access a ByteArray object
     * to ensure that you are reading valid data.</p>
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword available, bytes, position
     *
     * @playerversion Lite 4
     */
    public native function get bytesAvailable():uint;

    /**
     * Moves, or returns the current position, in bytes, of the file
     * pointer into the ByteArray object. This is the
     * point at which the next call to a read
     * method starts reading or a write
     * method starts writing.
     *
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.getFilePointer,
     *
     * @playerversion Lite 4
     */
    public native function get position():uint;
    public native function set position(offset:uint):void;

    /**
     * Denotes the default object encoding for the ByteArray class to use for a new ByteArray instance.
     * When you create a new ByteArray instance, the encoding on that instance starts
     * with the value of <code>defaultObjectEncoding</code>.
     * The <code>defaultObjectEncoding</code> property is initialized to <code>ObjectEncoding.AMF3</code>.
     *
     *
     * <p>When an object is written to or read from binary data, the <code>objectEncoding</code> value
     * is used to determine whether the ActionScript 3.0, ActionScript2.0, or ActionScript 1.0 format should be used. The value is a
     * constant from the ObjectEncoding class.</p>
     *
     * @see flash.net.ObjectEncoding ObjectEncoding class
     * @see flash.utils.ByteArray#objectEncoding
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.defaultObjectEncoding, defaultObjectEncoding
     *
     * @playerversion Lite 4
     */
    static public native function get defaultObjectEncoding():uint;
    static public native function set defaultObjectEncoding(version:uint):void;
    static private var _defaultObjectEncoding:uint;

    /**
     * Used to determine whether the ActionScript 3.0, ActionScript 2.0, or ActionScript 1.0 format should be
     * used when writing to, or reading from, a ByteArray instance. The value is a
     * constant from the ObjectEncoding class.
     *
     * @see flash.net.ObjectEncoding ObjectEncoding class
     * @see flash.utils.ByteArray#defaultObjectEncoding
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword ByteArray, ByteArray.objectEncoding, objectEncoding
     *
     * @playerversion Lite 4
     */
    public native function get objectEncoding():uint;
    public native function set objectEncoding(version:uint):void;

    /**
     * Changes or reads the byte order for the data; either <code>Endian.BIG_ENDIAN</code> or
     * <code>Endian.LITTLE_ENDIAN</code>.
     *
     * @see flash.utils.Endian Endian class
     * @playerversion Flash 9
     * @langversion 3.0
     * @helpid
     * @refpath
     * @keyword
     *
     * @playerversion Lite 4
     */
    public native function get endian():String;
    public native function set endian(type:String):void;
    
    [API(CONFIG::FP_10_0)]
    /**
     * Clears the contents of the byte array and resets the <code>length</code>
     * and <code>position</code> properties to 0. Calling this method explicitly
     * frees up the memory used by the ByteArray instance.
     *
     * @playerversion Flash 10
     * @playerversion AIR 1.5
     * @langversion 3.0
     *
     * @playerversion Lite 4
     */
    public native function clear():void;

    // Note: clients are free to replace with method returning non-string
    prototype.toJSON = function (k:String):* { return "ByteArray"; }

    // Bug 651641: we do not want toJSON enumerated.
    _dontEnumPrototype(prototype);


    /**
     * Atomic compare and swap of integer values in adjacent bytes in this byte array.
     *
     * Compares an expected value with the actual value in the byte array location
     * addressed by a start index measured in bytes.
     *
     * If the specified values at the given location are the same, the value given
     * by the newValue parameter is writen at the location specified by the byteIndex 
     * parameter and the previous value of that location is returned.
     *
     * Otherwise, the actual value is returned.
     * All of the above is performed in one atomic hardware transaction.
     * byteIndex must be a multiple of 4.
     *
     * @param byteIndex int containing the desired index at which the expectedValue 
     *        parameter should be compared.
     * @param expectedValue int containing the expected value of the integter to be
     *        replaced by the newValue parameter.
     * @param newValue int containint the new value to put into the location specified 
     *         by the byteIndex parameter.
     * @throws ArgumentError if byteIndex is not a multiple of 4 or negative
     * @return int containing the previous value at the specified location
     */
    [API(CONFIG::SWF_17)]
    public native function atomicCompareAndSwapIntAt(byteIndex :int, expectedValue: int, newValue :int) :int;

    /**
     * Atomic compare and change the length of this byte array.
     *
     * Compares an expected length with the actual length of this the byte array.
     * If the expected length argument and current ByteArray.length property are equal,
     * the length is changed to the value specified in the newLength parameter.
     *
     * The compare of the parameter and the update of the length all occur in a single atomic 
     * transaction.
     *
     * @param expectedLength int containint the expected value of the ByteArray's length. if this value is 
     *        equal to the ByteArray.length property, the length will be changed to the value
     *        specified by the newLength argument.
     * @param newLength int containing the the length the ByteArray should be after the
     *        operation succeeds.
     * @return int containint the previous value of ByteArray.length
     */
    [API(CONFIG::SWF_17)]
    public native function atomicCompareAndSwapLength(expectedLength: int, newLength :int) :int;    

	
	/**
	 * Every byte array can either be "non-shareable" or "shareable".
	 * This flag indicates which is the case for this byte array.
	 * The initial value is always 'false', indicating "non-shareable".
	 *
	 * If this byte array is non-shareable,
	 * then passing it as an argument to MessageChannel.send to another worker
	 * or via Worker.setStartArgument()
	 * will always create a complete byte array copy,
	 * including the backing storage for the byte array's contents.
	 *
	 * If this byte array is shareable,
	 * then passing it as an argument to MessageChannel.send to another worker
	 * or via Worker.setStartArgument()
	 * will result in a byte array object in the remote worker
	 * which uses the identical underlying storage buffer for it's contents.
	 * Then both the local and the remote byte array share their content.
	 *
	 * Concurrent access to a shared byte array from multiple workers is subject to races.
	 * For concurrency control, you can use the compare-and-swap facility in this class
	 * and/or the dedicated mechanisms in package 'flash.concurrent'.
	 *
	 * @returns whether this byte array is backed by storage only accessible
	 * in this worker and whether passing it on to another worker is done by copying
     *
     * @langversion 3.0
     * @playerversion Flash 11.4	
     * @playerversion AIR 3.4
	 */
	[API(CONFIG::SWF_17)]
	public native function get shareable() :Boolean;
	
	/**
	 * Sets the value of the 'shareable' flag described above.
	 *
	 * When setting the flag to 'true',
	 * this merely affects subsequent MessageChannel or Worker.setStartArgument() calls involving
	 * this byte array as an argument as described above.
	 * Otherwise there is no effect besides the flag's altered value.
	 *
	 * When setting the flag to 'false', if its previous value was 'true',
	 * the byte array's contents is copied into a new backing storage buffer
	 * and this buffer is immediately assigned to the byte array
	 * for use by any subsequent operations.
	 * Thus the sharing of contents with other workers is terminated and
	 * subsequent uses of this byte array as MessageChannel call argument lead
	 * to backing storage buffer copying.
     *
     * @langversion 3.0
     * @playerversion Flash 11.4	
     * @playerversion AIR 3.4
	 */
	[API(CONFIG::SWF_17)]
	public native function set shareable(newValue :Boolean) :void;
};



/*
 * [ggrossman 04/07/05] API SCRUB
 *
 * - _ByteArray_ now implements the _IDataInput_ and _IDataOutput_
 *   interfaces, as described in the Low Level Data specification.
 *
 * - Method _available()_ changed to accessor _bytesAvailable_
 * - Method _getFilePointer()_ changed to accessor _position_
 * - Method _seek()_ changed to accessor _position_
 * - Method _readUnsignedByte()_ now returns type _uint_
 * - Method _readUnsignedShort()_ now returns type _uint_
 *
 * - Renamed _flash_transient_ namespace to _transient_ and
 *   put in _flash.net_ package
 *
 * - Moved _registerClass_ to the _flash.net_ package.
 *
 * - Moved the _ObjectEncoding_ class from _flash.utils_ to the
 *   _flash.net_ package.
 *
 * - [srahim 04/05/05] Doc scrub
 */

}
