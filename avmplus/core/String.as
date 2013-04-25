/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package
{
    // instancegc protocol is custom (primitive value).

    [native(cls="StringClass", classgc="exact", instance="String", methods="auto", construct="override")]
    public final class String extends Object
    {
        // String.length = 1 per ES3
        // E262 {ReadOnly, DontDelete, DontEnum }
        public static const length:int = 1;

        AS3 native static function fromCharCode(...charcodes):String;

        // Bugzilla 573456: Optimize for the one-argument common case,
        // and for a small number of additional arguments.  The standard
        // rest arguments optimization ensures that the rest array is
        // not allocated.

        String.fromCharCode = function(...charcodes)
        {
            var argc:uint = charcodes.length;

            if (argc == 1)
                return AS3::fromCharCode(charcodes[0]);

            var s:String  = "";
            var i:uint    = 0;
            while (i < argc) {
                s += AS3::fromCharCode(charcodes[i]);
                i++;
            }
            return s;
        }

        // E262 {DontEnum, DontDelete, ReadOnly}
        public native function get length():int

        // indexOf and other _ functions get early bound by JIT
        private native function _indexOf(s:String, i:int=0):int // special-cased in Verifier, don't remove
        AS3 native function indexOf(s:String="undefined", i:Number=0):int
        prototype.indexOf = function(s:String="undefined", i:Number=0):int
        {
            return String(this).AS3::indexOf(s, i)
        }

        // lastIndexOf
        private native function _lastIndexOf(s:String, i:int=0x7fffffff):int // special-cased in Verifier, don't remove
        AS3 native function lastIndexOf(s:String="undefined", i:Number=0x7FFFFFFF):int

        prototype.lastIndexOf = function(s:String="undefined", i:Number=0x7fffffff):int
        {
            return String(this).AS3::lastIndexOf(s, i)
        }

        // charAt
        AS3 native function charAt(i:Number=0):String

        prototype.charAt = function(i:Number=0):String
        {
            return String(this).AS3::charAt(i)
        }

        // charCodeAt
        AS3 native function charCodeAt(i:Number=0):Number

        prototype.charCodeAt = function(i:Number=0):Number
        {
            return String(this).AS3::charCodeAt(i)
        }

        // concat
        AS3 function concat(...args):String
        {
            var s:String = this
            for (var i:uint = 0, n:uint = args.length; i < n; i++)
                s = s + String(args[i])
            return s
        }

        prototype.concat = function(... args):String
        {
            var s:String = String(this)
            for (var i:uint = 0, n:uint = args.length; i < n; i++)
                s = s + String(args[i])
            return s
        }

        AS3 native function localeCompare(other:*=void 0):int
        prototype.localeCompare = function(other:*=void 0):int
        {
            return String(this).AS3::localeCompare(other)
        }

        // match
        // P can be a RegEx or is coerced to a string (and then RegEx constructor is called)
        private static native function _match(s:String, p):Array
        AS3 function match(p=void 0):Array
        {
            return _match(this, p)
        }
        prototype.match = function(p=void 0):Array
        {
            return _match(String(this), p)
        }

        // replace
        // p is a RegEx or string
        // repl is a function or coerced to a string
        private static native function _replace(s:String, p, repl):String
        AS3 function replace(p=void 0, repl=void 0):String
        {
            return _replace(this, p, repl)
        }
        prototype.replace = function(p=void 0, repl=void 0):String
        {
            return _replace(String(this), p, repl)
        }

        // search
        // P can be a RegEx or is coerced to a string (and then RegEx constructor is called)
        private static native function _search(s:String, p):int
        AS3 function search(p=void 0):int
        {
            return _search(this, p)
        }

        prototype.search = function(p=void 0):int
        {
            return _search(String(this), p)
        }

        // slice
        private native function _slice(start:int=0, end:int=0x7fffffff):String // special-cased in Verifier, don't remove
        AS3 native function slice(start:Number=0, end:Number=0x7fffffff):String
        prototype.slice = function(start:Number=0, end:Number=0x7fffffff):String
        {
            return String(this).AS3::slice(start, end)
        }

        // This is a static helper since it depends on AvmCore which String objects
        // don't have access to.
        // delim can be a RegEx or is coerced to a string (and then RegEx constructor is called)
        private static native function _split(s:String, delim, limit:uint):Array
        AS3 function split(delim=void 0, limit=0xffffffff):Array
        {
            // ECMA compatibility - limit can be undefined
            if (limit == undefined)
                limit = 0xffffffff;
            return _split(this, delim, limit)
        }
        prototype.split = function(delim=void 0, limit=0xffffffff):Array
        {
            // ECMA compatibility - limit can be undefined
            if (limit == undefined)
                limit = 0xffffffff;
            return _split(String(this), delim, limit)
        }

        // substring
        private native function _substring(start:int=0, end:int=0x7fffffff):String // special-cased in Verifier, don't remove
        AS3 native function substring(start:Number=0, end:Number=0x7fffffff):String
        prototype.substring = function(start:Number=0, end:Number=0x7fffffff):String
        {
            return String(this).AS3::substring(start, end)
        }

        // substr
        private native function _substr(start:int=0, end:int=0x7fffffff):String // special-cased in Verifier, don't remove
        AS3 native function substr(start:Number=0, len:Number=0x7fffffff):String
        prototype.substr = function(start:Number=0, len:Number=0x7fffffff):String
        {
            return String(this).AS3::substr(start, len)
        }

        AS3 native function toLowerCase():String
        AS3 function toLocaleLowerCase():String
        {
            return this.AS3::toLowerCase();
        }

        prototype.toLowerCase = prototype.toLocaleLowerCase =
        function():String
        {
            return String(this).AS3::toLowerCase()
        }

        AS3 native function toUpperCase():String
        AS3 function toLocaleUpperCase():String
        {
            return this.AS3::toUpperCase();
        }

        prototype.toUpperCase = prototype.toLocaleUpperCase =
        function():String
        {
            return String(this).AS3::toUpperCase()
        }

        AS3 function toString():String { return this }
        AS3 function valueOf():String { return this }

        prototype.toString = function():String
        {
            if (this === prototype)
                return ""

            if (!(this is String))
                Error.throwError( TypeError, 1004 /*kInvokeOnIncompatibleObjectError*/, "String.prototype.toString" );

            return this
        }

        prototype.valueOf = function()
        {
            if (this === prototype)
                return ""

            if (!(this is String))
                Error.throwError( TypeError, 1004 /*kInvokeOnIncompatibleObjectError*/, "String.prototype.valueOf" );

            return this
        }

        // Dummy constructor function - This is neccessary so the compiler can do arg # checking for the ctor in strict mode
        // The code for the actual ctor is in StringClass::construct in the avmplus
        public function String(value = "")
        {}

        _dontEnumPrototype(prototype);
    }
}
