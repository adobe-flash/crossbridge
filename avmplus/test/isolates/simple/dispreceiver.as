/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import flash.system.PromiseChannel;
import flash.system.Worker;
import flash.utils.ByteArray;

print('hello from receiver');
var m1 = PromiseChannel.initial[0];
var m2 = PromiseChannel.initial[1];
var m3 = PromiseChannel.initial[2];

class Foo {
    var x:int = 10;
};
Worker.registerClassAlias("Foo", Foo);


m1.responder = function(arg) {
    if (arg is ByteArray)
        print("responder m1 got ByteArray");
    else
        print("responder m1 got result " + arg);
}

m2.responder = function(arg) {
    if (arg is ByteArray)
        print("responder m2 got ByteArray");
    else
        print("responder m2 got result " + arg);
}

trace('serious');

var count = 0;
try {
while (true) {
    PromiseChannel.runResponders();
    count ++;
}
} catch(e) {
    print('ran', count);
    throw e;
}