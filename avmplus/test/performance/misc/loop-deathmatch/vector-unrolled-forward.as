/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;

var DESC = "Vector forward int sum unrolled loop, 2048 elements";

function main()
{
    var then:uint = getTimer();
    var v: Vector.<int> = new Vector.<int>(2048);

    for ( var i:int=0 ; i < 2048 ; i++ )
        v[i] = 1;

    for ( var outer:int = 0 ; outer < 10000 ; outer++ ) {
        var sum:int=0;
        for ( var inner:int = 0 ; inner < 2048 ; inner += 8 )
            sum += v[inner] + v[inner+1] + v[inner+2] + v[inner+3] + v[inner+4] + v[inner+5] + v[inner+6] + v[inner+7];
    }

    if (sum != 2048)
        throw DESC + ": Unexpected result: " + sum;
    print((getTimer() - then) + "ms    " + DESC);
}
main();
