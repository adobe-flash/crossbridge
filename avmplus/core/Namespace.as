/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package
{
    // instancegc protocol is custom (primitive value).
    
    [native(cls="NamespaceClass", classgc="exact", instance="Namespace", methods="auto", construct="override")]
    public final class Namespace
    {
        // E262 {ReadOnly,DontDelete,DontEnum}
        // E357 length = 2
        public static const length = 2

        // E357 {DontDelete, ReadOnly}
        public native function get prefix()

        // E357 {DontDelete, ReadOnly}
        public native function get uri():String

        AS3 function valueOf():String { return uri }

        prototype.valueOf = function():String
        {
            if (this === prototype) return ""
            var n:Namespace = this
            return n.uri;
        }

        AS3 function toString():String
        {
            return uri
        }

        prototype.toString = function ():String
        {
            if (this === prototype) return ""
            var n:Namespace = this
            return n.AS3::toString();
        }

        // Dummy constructor function - This is neccessary so the compiler can do arg # checking for the ctor in strict mode
        // The code for the actual ctor is in NamespaceClass::construct in the avmplus
        public function Namespace(prefix = void 0, uri = void 0 )
        {}

        _dontEnumPrototype(prototype);
    }
}
