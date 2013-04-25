/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.utils
{

/**
 * The IExternalizable interface provides control over serialization of a class as it is encoded
 * into a data stream. The <code>writeExternal()</code> and
 * <code>readExternal()</code> methods of the IExternalizable interface are implemented by a class to allow customization
 * of the contents and format of the data stream (but not the classname or type) for an object and its supertypes.
 * Each individual class must serialize and reconstruct the state of its instances. These methods must be symmetrical with
 * the supertype to save its state. These methods supercede the native Action Message Format (AMF) serialization behavior.
 * <p>If a class does not implement, nor inherits from a class which implements, the IExternalizable interface, then an instance
 * of the class will be serialized using the default mechanism of public members only. As a result, private, internal, and
 * protected members of a class will not be available.</p>
 * <p>To serialize private members, a class must use the IExternalizable interface.
 * For example, the following class will not serialize any of its members because they are private:</p>
 * <listing>
 * class Example {
 *
 *       private var one:int;
 *       private var two:int;
 * }
 * </listing>
 * <p>However, if you implement the IExternalizable interface, you can write to, and read from, the data stream the private
 * members of the class as follows:</p>
 * <listing>
 * class Example implement IExternalizable {
 *
 *       private var one:int;
 *       private var two:int;
 *
 *       public function writeExternal(output:IDataOutput) {
 *            output.writeInt(one);
 *            output.writeInt(two);
 *       }
 *
 *       public function readExternal(input:IDataInput) {
 *            one = input.readInt();
 *            two = input.readInt();
 *       }
 * }
 * </listing>
 * <p><strong>Note:</strong>  If a class implements IExternalizable the default serialization no longer applies to instances
 * of that class. If that class inherits public members from a super class, you must carefully manage those members as well.</p>
 * <p>When a subclass of a class implementing IExternalizable has private members of its own, the subclass must override the
 * methods of IExternalizable, as follows:</p>
 * <listing>
 * public class Base implements IExternalizable {
 *
 *      private var one:Boolean;
 *
 *      public function writeExternal(output:IDataOutput):void {
 *          output.writeBoolean(one);
 *      }
 *
 *      public function readExternal(input:IDataInput):void {
 *          one = input.readBoolean();
 *      }
 * }
 *
 * public class Example extends Base {
 *
 *      private var one:String;
 *
 *
 *      public override function writeExternal(output:IDataOutput):void {
 *          super.writeExternal(output);
 *          output.writeUTF(one);
 *      }
 *
 *      public override function readExternal(input:IDataInput):void {
 *          super.readExternal(input);
 *          one = input.readUTF();
 *      }
 * }
 * </listing>
 * <p>The IExternalizable interface can also be used to compress data before writing it to a data stream.
 * For example:</p>
 * <listing>
 * class Example implements IExternalizable {
 *
 *      public var one:Boolean;
 *      public var two:Boolean;
 *      public var three:Boolean;
 *      public var four:Boolean;
 *      public var five:Boolean;
 *      public var six:Boolean;
 *      public var seven:Boolean;
 *      public var eight:Boolean;
 *
 *      public function writeExternal(output:IDataOutput) {
 *          var flag:int = 0;
 *
 *          if (one) flag |= 1;
 *          if (two) flag |= 2;
 *          if (three) flag |= 4;
 *          if (four) flag |= 8;
 *          if (five) flag |= 16;
 *          if (six) flag |= 32;
 *          if (seven) flag |= 64;
 *          if (eight) flag |= 128;
 *
 *          output.writeByte(flag);
 *      }
 *
 *      public function readExternal(input:IDataInput) {
 *          var flag:int = input.readByte();
 *
 *          one = (flag &amp; 1) != 0;
 *          two = (flag &amp; 2) != 0;
 *          three = (flag &amp; 4) != 0;
 *          four = (flag &amp; 8) != 0;
 *          five = (flag &amp; 16) != 0;
 *          six = (flag &amp; 32) != 0;
 *          seven = (flag &amp; 64) != 0;
 *          eight = (flag &amp; 128) != 0;
 *      }
 * }
 * </listing>
 * @see flash.net.ObjectEncoding
 * @playerversion Flash 9
 * @langversion 3.0
 *
 *  @playerversion Lite 4
 */
public interface IExternalizable
{
    /**
     * A class implements this method to encode itself for a data stream by calling the methods of the IDataOutput
     * interface.
     * @param output The name of the class that implements the IDataOutput interface.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    function writeExternal(output:IDataOutput):void;
    
    /**
     * A class implements this method to decode itself from a data stream by calling the methods of the IDataInput
     * interface. This method must read the values in the same sequence and with the same types as
     * were written by the <code>writeExternal()</code> method.
     * @param input The name of the class that implements the IDataInput interface.
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */
    function readExternal(input:IDataInput):void;
}

}
