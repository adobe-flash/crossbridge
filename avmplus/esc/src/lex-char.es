/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

use default namespace Char,
    namespace Char;

const EOS = 0;
const a = "a".charCodeAt(0);
const b = "b".charCodeAt(0);
const c = "c".charCodeAt(0);
const d = "d".charCodeAt(0);
const e = "e".charCodeAt(0);
const f = "f".charCodeAt(0);
const g = "g".charCodeAt(0);
const h = "h".charCodeAt(0);
const i = "i".charCodeAt(0);
const j = "j".charCodeAt(0);
const k = "k".charCodeAt(0);
const l = "l".charCodeAt(0);
const m = "m".charCodeAt(0);
const n = "n".charCodeAt(0);
const o = "o".charCodeAt(0);
const p = "p".charCodeAt(0);
const q = "q".charCodeAt(0);
const r = "r".charCodeAt(0);
const s = "s".charCodeAt(0);
const t = "t".charCodeAt(0);
const u = "u".charCodeAt(0);
const v = "v".charCodeAt(0);
const w = "w".charCodeAt(0);
const x = "x".charCodeAt(0);
const y = "y".charCodeAt(0);
const z = "z".charCodeAt(0);
const A = "A".charCodeAt(0);
const B = "B".charCodeAt(0);
const C = "C".charCodeAt(0);
const D = "D".charCodeAt(0);
const E = "E".charCodeAt(0);
const F = "F".charCodeAt(0);
const G = "G".charCodeAt(0);
const H = "H".charCodeAt(0);
const I = "I".charCodeAt(0);
const J = "J".charCodeAt(0);
const K = "K".charCodeAt(0);
const L = "L".charCodeAt(0);
const M = "M".charCodeAt(0);
const N = "N".charCodeAt(0);
const O = "O".charCodeAt(0);
const P = "P".charCodeAt(0);
const Q = "Q".charCodeAt(0);
const R = "R".charCodeAt(0);
const S = "S".charCodeAt(0);
const T = "T".charCodeAt(0);
const U = "U".charCodeAt(0);
const V = "V".charCodeAt(0);
const W = "W".charCodeAt(0);
const X = "X".charCodeAt(0);
const Y = "Y".charCodeAt(0);
const Z = "Z".charCodeAt(0);
const Zero = "0".charCodeAt(0);
const One = "1".charCodeAt(0);
const Two = "2".charCodeAt(0);
const Three = "3".charCodeAt(0);
const Four = "4".charCodeAt(0);
const Five = "5".charCodeAt(0);
const Six = "6".charCodeAt(0);
const Seven = "7".charCodeAt(0);
const Eight = "8".charCodeAt(0);
const Nine = "9".charCodeAt(0);
const Dot = ".".charCodeAt(0);
const Bang = "!".charCodeAt(0);
const Equal = "=".charCodeAt(0);
const Percent = "%".charCodeAt(0);
const Ampersand = "&".charCodeAt(0);
const Asterisk = "*".charCodeAt(0);
const Plus = "+".charCodeAt(0);
const Dash = "-".charCodeAt(0);
const Slash = "/".charCodeAt(0);
const BackSlash = "\\".charCodeAt(0);
const Comma = ",".charCodeAt(0);
const Colon = ":".charCodeAt(0);
const Semicolon = ";".charCodeAt(0);
const LeftAngle = "<".charCodeAt(0);
const RightAngle = ">".charCodeAt(0);
const Caret = "^".charCodeAt(0);
const Bar = "|".charCodeAt(0);
const QuestionMark = "?".charCodeAt(0);
const LeftParen = "(".charCodeAt(0);
const RightParen = ")".charCodeAt(0);
const LeftBrace = "{".charCodeAt(0);
const RightBrace = "}".charCodeAt(0);
const LeftBracket = "[".charCodeAt(0);
const RightBracket = "]".charCodeAt(0);
const Tilde = "~".charCodeAt(0);
const At = "@".charCodeAt(0);
const SingleQuote = "'".charCodeAt(0);
const DoubleQuote = "\"".charCodeAt(0);
const UnderScore = "_".charCodeAt(0);
const Dollar = "$".charCodeAt(0);
const Space = " ".charCodeAt(0);
const Tab = "\t".charCodeAt(0);
const VerticalTab = "\v".charCodeAt(0);
const Newline = "\n".charCodeAt(0);
const CarriageReturn = "\r".charCodeAt(0);
const Backspace = "\b".charCodeAt(0);
const Formfeed = "\f".charCodeAt(0);
const UnicodeLS = 0x2028;
const UnicodePS = 0x2029;

function isUnicodeIdentifierStart(c) {
    if (c >= Char::A && c <= Char::Z) 
        return true;
    if (c >= Char::a && c <= Char::z) 
        return true;
    if (c == Char::UnderScore) 
        return true;
    if (c == Char::Dollar) 
        return true;
    // FIXME: More Unicode here!
    return false;
}

function isUnicodeIdentifierPart(c) {
    if (c >= Char::A && c <= Char::Z) 
        return true;
    if (c >= Char::a && c <= Char::z) 
        return true;
    if (c >= Char::Zero && c <= Char::Nine) 
        return true;
    if (c == Char::UnderScore) 
        return true;
    if (c == Char::Dollar) 
        return true;
    // FIXME: More Unicode here!
    return false;
}

// Whitespace except BOM

function isUnicodeWhitespace(c) {
    switch (c) {
    case 0x0009:
    case 0x000A:
    case 0x000B:
    case 0x000D:
    case 0x0012:
    case 0x0020:
    case 0x00A0:
    case 0x1680:
    case 0x180e:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2007:
    case 0x2008:
    case 0x2009:
    case 0x200a:
    case 0x202f:
    case 0x205f:
    case 0x3000:
        return true;
    default:
        return false;
    }
}
