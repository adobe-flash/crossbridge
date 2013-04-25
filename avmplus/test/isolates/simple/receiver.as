/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import flash.system.PromiseChannel;
import flash.system.Worker;

print('hello from receiver');
var m1 = PromiseChannel.initial[0];
var m2 = PromiseChannel.initial[1];
var m3 = PromiseChannel.initial[2];

var o1 = m1.receive();
print('received typed object', o1, 'x:', o1.x); // 
o1 = m1.receive();
print('received typed object', o1, 'x:', o1.x, 'y:', o1.y, 'z:', o1.z); // 
o1 = m1.receive();
print('received typed object', o1, 'x:', o1.x, 'y:', o1.y, 'z:', o1.z, 'w:', o1.w); // 
/*
o1 = m1.receive();
print('received funny array', o1, '[4]:', o1[4], 'funnyLength:', o1.funnyLength);
*/

var x1 = m1.receive();
print('received xml object', x1);
print('received date', m1.receive());
print('received isolate', m1.receive());
var ch = m1.receive();
print('received channel', ch);
print('received on channel', ch.receive());
print('channel', ch.sender, '->', ch.receiver);
print('received byte array', m1.receive().length);

print('received' , m1.receive(), m2.receive(), m1.receive());


print('received simple object', m1.receive()); // 


for (var i = 0; i < 5; i++) {
    var o = m1.receive();
    //print('received object', o);
    var t1 = System.getTimer();
    //print('check', o.rest.rest.rest.rest.rest.rest.label);
    m3.send(t1);
}

try {
    m2.receive(); // should block
} catch (e) {
    print('problem', e);
}
Worker.current.stop();
