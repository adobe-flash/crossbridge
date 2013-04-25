/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.net
{

    /**
     * This interface is used with the IDynamicPropertyOutput interface to control
     * the serialization of dynamic properties of dynamic objects. To use this interface,
     * assign an object that implements the IDynamicPropertyWriter interface to
     * the <code>ObjectEncoding.dynamicPropertyWriter</code> property.
     *
     * @see IDynamicPropertyOutput
     * @see ObjectEncoding#dynamicPropertyWriter
     * @playerversion Flash 9
     * @langversion 3.0
     *
     *  @playerversion Lite 4
     */

    public interface IDynamicPropertyWriter
    {
        /**
         * Writes the name and value of an IDynamicPropertyOutput object to an object with
         * dynamic properties. If <code>ObjectEncoding.dynamicPropertyWriter</code> is set,
         * this method is invoked for each object with dynamic properties.
         *
         * @param obj The object to write to.
         * @param output The IDynamicPropertyOutput object that contains the name and value
         * to dynamically write to the object.
         *
         * @see IDynamicPropertyOutput
         * @see ObjectEncoding#dynamicPropertyWriter
         *
         * @playerversion Flash 9
         * @langversion 3.0
         *
         *  @playerversion Lite 4
         */
        function writeDynamicProperties(obj:Object, output:IDynamicPropertyOutput):void;
    }
     
}
