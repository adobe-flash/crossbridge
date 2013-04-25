/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.net
{

    /**
     * This interface controls the serialization of dynamic properties of dynamic objects.
     * You use this interface with the IDynamicPropertyWriter interface
     * and the <code>ObjectEncoding.dynamicPropertyWriter</code> property.
     *
     * @see IDynamicPropertyWriter
     * @see ObjectEncoding#dynamicPropertyWriter
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */

    public interface IDynamicPropertyOutput
    {
        /**
         * Adds a dynamic property to the binary output of a serialized object.
         * When the object is subsequently read (using a method such as
         * <code>readObject</code>), it contains the new property.
         * You can use this method
         * to exclude properties of dynamic objects from serialization; to write values
         * to properties of dynamic objects; or to create new properties
         * for dynamic objects.
         *
         * @param name The name of the property. You can use this parameter either to specify
         *        the name of an existing property of the dynamic object or to create a
         *        new property.
         *
         * @param value The value to write to the specified property.
         *
         * @see IDynamicPropertyWriter
         * @see ObjectEncoding#dynamicPropertyWriter
         * @playerversion Flash 9
         * @langversion 3.0
         *
         *  @playerversion Lite 4
         */
        function writeDynamicProperty(name:String, value:*):void;
    }

}
