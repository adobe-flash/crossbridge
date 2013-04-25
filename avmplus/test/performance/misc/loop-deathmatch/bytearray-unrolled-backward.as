/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.ByteArray;
import avmplus.*;

var DESC = "ByteArray backward int sum unrolled loop, 2048 elements";

function main()
{
    var then:uint = getTimer();
    var v: ByteArray = new ByteArray;

    v.endian = "littleEndian"; // Specialized for x86
    v.length = 8192;
    v.position = 0;
    for ( var i:int=0 ; i < 2048 ; i++ )
        v.writeInt(1);

    for ( var outer:int = 0 ; outer < 10000 ; outer++ ) {
        var sum:int=0;
        for ( var inner:int = 8192 ; inner > 0 ; ) {
            v.position = inner-4;
            sum += v.readInt();
            v.position = inner-8;
            sum += v.readInt();
            v.position = inner-12;
            sum += v.readInt();
            v.position = inner-16;
            sum += v.readInt();
            v.position = inner-20;
            sum += v.readInt();
            v.position = inner-24;
            sum += v.readInt();
            v.position = inner-28;
            sum += v.readInt();
            v.position = inner-32;
            sum += v.readInt();
            inner -= 32;
        }
    }

    if (sum != 2048)
        throw DESC + ": Unexpected result: " + sum;
    print((getTimer() - then) + "ms    " + DESC);
}
main();
