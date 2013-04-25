/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;

var DESC = "Read long UTF string from length-prefixed ByteArray, store in local with type String.";
include "driver.as"

function bytearray_read_utf(iter: int, a: ByteArray, b: ByteArray): int
{
    var tmp1:String;
    var tmp2:String;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        a.position = 0;
        b.position = 0;
        for ( var j:int = 0 ; j < 1000 ; j++ ) {
            tmp1 = a.readUTF();
            tmp2 = b.readUTF();
        }
    }
    return tmp1+tmp2;
}

var ba1: ByteArray = new ByteArray();
var ba2: ByteArray = new ByteArray();
(function () {
    for ( var i=0 ; i < 1000 ; i++ ) {
        ba1.writeUTF("It was the best of times, it was the worst of times, it was the age of wisdom, it was the age of foolishness, it was the epoch of belief, it was the epoch of incredulity, it was the season of Light, it was the season of Darkness, it was the spring of hope, it was the winter of despair, we had everything before us...");
        ba2.writeUTF("Ja vi elsker dette landet som det stiger frem furet vaerbitt over vannet med de tusen hjem elsker elsker det og tenker paa vaar far og mor og den saganatt som senker senker droemme paa vaar jord");
    }
 })();
TEST(function () { bytearray_read_utf(100, ba1, ba2); }, "bytearray-read-utf-2");
