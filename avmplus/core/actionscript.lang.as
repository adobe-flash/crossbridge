/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package
{
    public namespace AS3 = "http://adobe.com/AS3/2006/builtin";

    // Query whether a particular bugfix is in effect for this object.
    // information. Note that we use VM_INTERNAL to hide it from non-builtin code.
    [native("Toplevel::bugzilla")]
    [API(CONFIG::VM_INTERNAL)]
    public native function bugzilla(n:int):Boolean;

    /**
    * @name Toplevel Function Properties
    * Function properties of the global object (ECMA 15.1.2)
    */

    // {DontEnum} length=1
    [native("Toplevel::decodeURI")]
    public native function decodeURI(uri:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::decodeURIComponent")]
    public native function decodeURIComponent(uri:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::encodeURI")]
    public native function encodeURI(uri:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::encodeURIComponent")]
    public native function encodeURIComponent(uri:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::isNaN")]
    public native function isNaN(n:Number = void 0):Boolean

    // {DontEnum} length=1
    [native("Toplevel::isFinite")]
    public native function isFinite(n:Number = void 0):Boolean

    // {DontEnum} length=1
    [native("Toplevel::parseInt")]
    public native function parseInt(s:String = "NaN", radix:int=0):Number

    // {DontEnum} length=1
    [native("Toplevel::parseFloat")]
    public native function parseFloat(str:String = "NaN"):Number


    /**
    * @name ECMA-262 Appendix B.2 extensions
    * Extensions to ECMAScript, in ECMA-262 Appendix B.2
    */

    // {DontEnum} length=1
    [native("Toplevel::escape")]
    public native function escape(s:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::unescape")]
    public native function unescape(s:String="undefined"):String

    // {DontEnum} length=1
    [native("Toplevel::isXMLName")]
    public native function isXMLName(str=void 0):Boolean    // moved here from XML.as

    // value properties of global object (ECMA 15.1.1)

    // in E262, these are var {DontEnum,DontDelete}  but not ReadOnly
    // but in E327, these are {ReadOnly, DontEnum, DontDelete}
    // we choose to make them const ala E327

    // The initial value of NaN is NaN (section 8.5).
    // E262 { DontEnum, DontDelete}
    // E327 { DontEnum, DontDelete, ReadOnly}
    public const NaN:Number = 0/0

    // The initial value of Infinity is +8 (section 8.5).
    // E262 { DontEnum, DontDelete}
    // E327 { DontEnum, DontDelete, ReadOnly}
    public const Infinity:Number = 1/0

    // The initial value of undefined is undefined (section 8.1).
    // E262 { DontEnum, DontDelete}
    // E327 { DontEnum, DontDelete, ReadOnly}
    public const undefined = void 0
}
