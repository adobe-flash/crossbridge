/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package
{

// Note, no instances.
// FIXME: the 'instance' spec here is wrong, workaround for a bug in nativegen.py?

[native(cls="MathClass", classgc="exact", instance="double", methods="auto", construct="override")]
public final class Math
{
    // the value properties of math are {DontEnum,DontDelete,ReadOnly}
    public static const E       :Number = 2.718281828459045
    public static const LN10    :Number = 2.302585092994046
    public static const LN2     :Number = 0.6931471805599453
    public static const LOG10E  :Number = 0.4342944819032518
    public static const LOG2E   :Number = 1.442695040888963387
    public static const PI      :Number = 3.141592653589793
    public static const SQRT1_2 :Number = 0.7071067811865476
    public static const SQRT2   :Number = 1.4142135623730951

    // min and max with 2 args are so common we overload these
    // note, these appear to be unused, but are special-cased (by name) in Verifier
    // in certain cases. Don't remove them.
    native private static function _min    (x:Number, y:Number):Number
    native private static function _max    (x:Number, y:Number):Number

    // wrappers for the above math methods.  these do automatic
    // conversion of their arguments.

    public native static function abs   (x:Number)   :Number
    public native static function acos  (x:Number)   :Number
    public native static function asin  (x:Number)   :Number
    public native static function atan  (x:Number)   :Number
    public native static function ceil  (x:Number)   :Number
    public native static function cos   (x:Number)   :Number
    public native static function exp   (x:Number)   :Number
    public native static function floor (x:Number)   :Number
    public native static function log   (x:Number)   :Number
    public native static function round (x:Number)   :Number
    public native static function sin   (x:Number)   :Number
    public native static function sqrt  (x:Number)   :Number
    public native static function tan   (x:Number)   :Number

    public native static function atan2 (y:Number, x:Number):Number
    public native static function pow   (x:Number, y:Number):Number

    private static const NegInfinity:Number = -1/0;
    public native static function max    (x:Number = NegInfinity, y:Number = NegInfinity, ... rest):Number
    public native static function min    (x:Number = Infinity, y:Number = Infinity, ... rest):Number

    public native static function random ():Number
}

}
