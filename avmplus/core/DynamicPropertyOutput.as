/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package flash.net
{

/**
 * This class is internal to flash.net, so it does not require ASDoc's
 *
 * When static ObjectEncoding.dynamicPropertyWriter is set with a non-null
 * IDynamicPropertyWriter, the native code will construct a DynamicPropertyOutput
 * to use as the parameter to IDynamicPropertyWriter.writeDynamicProperties().
 *
 */
        
    [native(cls="DynamicPropertyOutputClass", gc="exact", instance="DynamicPropertyOutputObject", methods="auto", construct="native")]
    internal class DynamicPropertyOutput implements IDynamicPropertyOutput
    {
        public native function writeDynamicProperty(name:String, value:*):void;
    }

}
