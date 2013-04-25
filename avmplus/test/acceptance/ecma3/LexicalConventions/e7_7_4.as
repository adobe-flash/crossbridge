/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "7.7.4";
//     var VERSION = "ECMA_1";
//     var TITLE   = "String Literals";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // StringLiteral:: "" and ''

    array[item++] = Assert.expectEq(  "\"\"",     "",     "" );
    array[item++] = Assert.expectEq(  "\'\'",     "",      '' );

    // DoubleStringCharacters:: DoubleStringCharacter :: EscapeSequence :: CharacterEscapeSequence
    array[item++] = Assert.expectEq(  "\\\"",        String.fromCharCode(0x0022),     "\"" );
    array[item++] = Assert.expectEq(  "\\\'",        String.fromCharCode(0x0027),     "\'" );
    array[item++] = Assert.expectEq(  "\\",         String.fromCharCode(0x005C),     "\\" );
    array[item++] = Assert.expectEq(  "\\b",        String.fromCharCode(0x0008),     "\b" );
    array[item++] = Assert.expectEq(  "\\f",        String.fromCharCode(0x000C),     "\f" );
    array[item++] = Assert.expectEq(  "\\n",        String.fromCharCode(0x000A),     "\n" );
    array[item++] = Assert.expectEq(  "\\r",        String.fromCharCode(0x000D),     "\r" );
    array[item++] = Assert.expectEq(  "\\t",        String.fromCharCode(0x0009),     "\t" );
    array[item++] = Assert.expectEq(  "\\v",        String.fromCharCode(0x000B),        "\v" );

//  following line commented out as it causes a compile time error
//    array[item++] = Assert.expectEq(  "\\444",      "444",                         "\444" );

    // DoubleStringCharacters:DoubleStringCharacter::EscapeSequence::HexEscapeSequence
/*
    array[item++] = Assert.expectEq(  "\\x0",      String.fromCharCode(0),         "\x0" );
    array[item++] = Assert.expectEq(  "\\x1",      String.fromCharCode(1),         "\x1" );
    array[item++] = Assert.expectEq(  "\\x2",      String.fromCharCode(2),         "\x2" );
    array[item++] = Assert.expectEq(  "\\x3",      String.fromCharCode(3),         "\x3" );
    array[item++] = Assert.expectEq(  "\\x4",      String.fromCharCode(4),         "\x4" );
    array[item++] = Assert.expectEq(  "\\x5",      String.fromCharCode(5),         "\x5" );
    array[item++] = Assert.expectEq(  "\\x6",      String.fromCharCode(6),         "\x6" );
    array[item++] = Assert.expectEq(  "\\x7",      String.fromCharCode(7),         "\x7" );
    array[item++] = Assert.expectEq(  "\\x8",      String.fromCharCode(8),         "\x8" );
    array[item++] = Assert.expectEq(  "\\x9",      String.fromCharCode(9),         "\x9" );
    array[item++] = Assert.expectEq(  "\\xA",      String.fromCharCode(10),         "\xA" );
    array[item++] = Assert.expectEq(  "\\xB",      String.fromCharCode(11),         "\xB" );
    array[item++] = Assert.expectEq(  "\\xC",      String.fromCharCode(12),         "\xC" );
    array[item++] = Assert.expectEq(  "\\xD",      String.fromCharCode(13),         "\xD" );
    array[item++] = Assert.expectEq(  "\\xE",      String.fromCharCode(14),         "\xE" );
    array[item++] = Assert.expectEq(  "\\xF",      String.fromCharCode(15),         "\xF" );

*/
    array[item++] = Assert.expectEq(  "\\xF0",      String.fromCharCode(240),         "\xF0" );
    array[item++] = Assert.expectEq(  "\\xE1",      String.fromCharCode(225),         "\xE1" );
    array[item++] = Assert.expectEq(  "\\xD2",      String.fromCharCode(210),         "\xD2" );
    array[item++] = Assert.expectEq(  "\\xC3",      String.fromCharCode(195),         "\xC3" );
    array[item++] = Assert.expectEq(  "\\xB4",      String.fromCharCode(180),         "\xB4" );
    array[item++] = Assert.expectEq(  "\\xA5",      String.fromCharCode(165),         "\xA5" );
    array[item++] = Assert.expectEq(  "\\x96",      String.fromCharCode(150),         "\x96" );
    array[item++] = Assert.expectEq(  "\\x87",      String.fromCharCode(135),         "\x87" );
    array[item++] = Assert.expectEq(  "\\x78",      String.fromCharCode(120),         "\x78" );
    array[item++] = Assert.expectEq(  "\\x69",      String.fromCharCode(105),         "\x69" );
    array[item++] = Assert.expectEq(  "\\x5A",      String.fromCharCode(90),         "\x5A" );
    array[item++] = Assert.expectEq(  "\\x4B",      String.fromCharCode(75),         "\x4B" );
    array[item++] = Assert.expectEq(  "\\x3C",      String.fromCharCode(60),         "\x3C" );
    array[item++] = Assert.expectEq(  "\\x2D",      String.fromCharCode(45),         "\x2D" );
    array[item++] = Assert.expectEq(  "\\x1E",      String.fromCharCode(30),         "\x1E" );
    array[item++] = Assert.expectEq(  "\\x0F",      String.fromCharCode(15),         "\x0F" );

    // string literals only take up to two hext digits.  therefore, the third character in this string
    // should be interpreted as a StringCharacter and not part of the HextEscapeSequence

    array[item++] = Assert.expectEq(  "\\xF0F",      String.fromCharCode(240)+"F",         "\xF0F" );
    array[item++] = Assert.expectEq(  "\\xE1E",      String.fromCharCode(225)+"E",         "\xE1E" );
    array[item++] = Assert.expectEq(  "\\xD2D",      String.fromCharCode(210)+"D",         "\xD2D" );
    array[item++] = Assert.expectEq(  "\\xC3C",      String.fromCharCode(195)+"C",         "\xC3C" );
    array[item++] = Assert.expectEq(  "\\xB4B",      String.fromCharCode(180)+"B",         "\xB4B" );
    array[item++] = Assert.expectEq(  "\\xA5A",      String.fromCharCode(165)+"A",         "\xA5A" );
    array[item++] = Assert.expectEq(  "\\x969",      String.fromCharCode(150)+"9",         "\x969" );
    array[item++] = Assert.expectEq(  "\\x878",      String.fromCharCode(135)+"8",         "\x878" );
    array[item++] = Assert.expectEq(  "\\x787",      String.fromCharCode(120)+"7",         "\x787" );
    array[item++] = Assert.expectEq(  "\\x696",      String.fromCharCode(105)+"6",         "\x696" );
    array[item++] = Assert.expectEq(  "\\x5A5",      String.fromCharCode(90)+"5",         "\x5A5" );
    array[item++] = Assert.expectEq(  "\\x4B4",      String.fromCharCode(75)+"4",         "\x4B4" );
    array[item++] = Assert.expectEq(  "\\x3C3",      String.fromCharCode(60)+"3",         "\x3C3" );
    array[item++] = Assert.expectEq(  "\\x2D2",      String.fromCharCode(45)+"2",         "\x2D2" );
    array[item++] = Assert.expectEq(  "\\x1E1",      String.fromCharCode(30)+"1",         "\x1E1" );
    array[item++] = Assert.expectEq(  "\\x0F0",      String.fromCharCode(15)+"0",         "\x0F0" );

    // G is out of hex range

    array[item++] = Assert.expectEq(  "\\xG",        "xG",                                 "\xG" );
    array[item++] = Assert.expectEq(  "\\xCG",       "xCG",                    "\xCG" );

    // DoubleStringCharacter::EscapeSequence::CharacterEscapeSequence::\ NonEscapeCharacter
    array[item++] = Assert.expectEq(  "\\a",    "a",        "\a" );
    array[item++] = Assert.expectEq(  "\\c",    "c",        "\c" );
    array[item++] = Assert.expectEq(  "\\d",    "d",        "\d" );
    array[item++] = Assert.expectEq(  "\\e",    "e",        "\e" );
    array[item++] = Assert.expectEq(  "\\g",    "g",        "\g" );
    array[item++] = Assert.expectEq(  "\\h",    "h",        "\h" );
    array[item++] = Assert.expectEq(  "\\i",    "i",        "\i" );
    array[item++] = Assert.expectEq(  "\\j",    "j",        "\j" );
    array[item++] = Assert.expectEq(  "\\k",    "k",        "\k" );
    array[item++] = Assert.expectEq(  "\\l",    "l",        "\l" );
    array[item++] = Assert.expectEq(  "\\m",    "m",        "\m" );
    array[item++] = Assert.expectEq(  "\\o",    "o",        "\o" );
    array[item++] = Assert.expectEq(  "\\p",    "p",        "\p" );
    array[item++] = Assert.expectEq(  "\\q",    "q",        "\q" );
    array[item++] = Assert.expectEq(  "\\s",    "s",        "\s" );
    array[item++] = Assert.expectEq(  "\\u",    "u",        "\u" );

    array[item++] = Assert.expectEq(  "\\w",    "w",        "\w" );
    array[item++] = Assert.expectEq(  "\\x",    "x",        "\x" );
    array[item++] = Assert.expectEq(  "\\y",    "y",        "\y" );
    array[item++] = Assert.expectEq(  "\\z",    "z",        "\z" );
    array[item++] = Assert.expectEq(  "\\9",    "9",        "\9" );

    array[item++] = Assert.expectEq(  "\\A",    "A",        "\A" );
    array[item++] = Assert.expectEq(  "\\B",    "B",        "\B" );
    array[item++] = Assert.expectEq(  "\\C",    "C",        "\C" );
    array[item++] = Assert.expectEq(  "\\D",    "D",        "\D" );
    array[item++] = Assert.expectEq(  "\\E",    "E",        "\E" );
    array[item++] = Assert.expectEq(  "\\F",    "F",        "\F" );
    array[item++] = Assert.expectEq(  "\\G",    "G",        "\G" );
    array[item++] = Assert.expectEq(  "\\H",    "H",        "\H" );
    array[item++] = Assert.expectEq(  "\\I",    "I",        "\I" );
    array[item++] = Assert.expectEq(  "\\J",    "J",        "\J" );
    array[item++] = Assert.expectEq(  "\\K",    "K",        "\K" );
    array[item++] = Assert.expectEq(  "\\L",    "L",        "\L" );
    array[item++] = Assert.expectEq(  "\\M",    "M",        "\M" );
    array[item++] = Assert.expectEq(  "\\N",    "N",        "\N" );
    array[item++] = Assert.expectEq(  "\\O",    "O",        "\O" );
    array[item++] = Assert.expectEq(  "\\P",    "P",        "\P" );
    array[item++] = Assert.expectEq(  "\\Q",    "Q",        "\Q" );
    array[item++] = Assert.expectEq(  "\\R",    "R",        "\R" );
    array[item++] = Assert.expectEq(  "\\S",    "S",        "\S" );
    array[item++] = Assert.expectEq(  "\\T",    "T",        "\T" );
    array[item++] = Assert.expectEq(  "\\U",    "U",        "\U" );
    array[item++] = Assert.expectEq(  "\\V",    "V",        "\V" );
    array[item++] = Assert.expectEq(  "\\W",    "W",        "\W" );
    array[item++] = Assert.expectEq(  "\\X",    "X",        "\X" );
    array[item++] = Assert.expectEq(  "\\Y",    "Y",        "\Y" );
    array[item++] = Assert.expectEq(  "\\Z",    "Z",        "\Z" );

    // DoubleStringCharacter::EscapeSequence::UnicodeEscapeSequence

    array[item++] = Assert.expectEq(   "\\u0020",  " ",        "\u0020" );
    array[item++] = Assert.expectEq(   "\\u0021",  "!",        "\u0021" );
    array[item++] = Assert.expectEq(   "\\u0022",  "\"",       "\u0022" );
    array[item++] = Assert.expectEq(   "\\u0023",  "#",        "\u0023" );
    array[item++] = Assert.expectEq(   "\\u0024",  "$",        "\u0024" );
    array[item++] = Assert.expectEq(   "\\u0025",  "%",        "\u0025" );
    array[item++] = Assert.expectEq(   "\\u0026",  "&",        "\u0026" );
    array[item++] = Assert.expectEq(   "\\u0027",  "'",        "\u0027" );
    array[item++] = Assert.expectEq(   "\\u0028",  "(",        "\u0028" );
    array[item++] = Assert.expectEq(   "\\u0029",  ")",        "\u0029" );
    array[item++] = Assert.expectEq(   "\\u002A",  "*",        "\u002A" );
    array[item++] = Assert.expectEq(   "\\u002B",  "+",        "\u002B" );
    array[item++] = Assert.expectEq(   "\\u002C",  ",",        "\u002C" );
    array[item++] = Assert.expectEq(   "\\u002D",  "-",        "\u002D" );
    array[item++] = Assert.expectEq(   "\\u002E",  ".",        "\u002E" );
    array[item++] = Assert.expectEq(   "\\u002F",  "/",        "\u002F" );
    array[item++] = Assert.expectEq(   "\\u0030",  "0",        "\u0030" );
    array[item++] = Assert.expectEq(   "\\u0031",  "1",        "\u0031" );
    array[item++] = Assert.expectEq(   "\\u0032",  "2",        "\u0032" );
    array[item++] = Assert.expectEq(   "\\u0033",  "3",        "\u0033" );
    array[item++] = Assert.expectEq(   "\\u0034",  "4",        "\u0034" );
    array[item++] = Assert.expectEq(   "\\u0035",  "5",        "\u0035" );
    array[item++] = Assert.expectEq(   "\\u0036",  "6",        "\u0036" );
    array[item++] = Assert.expectEq(   "\\u0037",  "7",        "\u0037" );
    array[item++] = Assert.expectEq(   "\\u0038",  "8",        "\u0038" );
    array[item++] = Assert.expectEq(   "\\u0039",  "9",        "\u0039" );


    return ( array );
}
