/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "6.3.6";
// var VERSION = "AS3";
// var TITLE   = "The bitwise not ~ operator";



var flt:float = new float(3.1413119f);
AddStrictTestCase("bitwise not on float", ~3, ~flt);

// float 12.375f in binary 01000001010001100000000000000000
// bitwise NOT of memory   10111110101110011111111111111111
// int value of memory     -1095106561
// ~12.375f should be bitwise NOT of 12
// 12 in binary            00000000 00000000 00000000 00001100
// bitwise NOT of 13       11111111 11111111 11111111 11110011
// int value of memory     -13
flt = 12.375f;
AddStrictTestCase("ToInt32 of float value instead of float memory representation", true, Number(~flt) != -1095106561);
AddStrictTestCase("~12.375f ", ~12, ~flt);


