/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import avmplus.File;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.PromiseChannel;

function makeCode(name) {
    return File.readByteArray(name);
}

var p = new Worker([makeCode("echo.abc")]);
var m1 = new PromiseChannel(Worker.current, p);
var m2 = new PromiseChannel(p, Worker.current);

p.startWithChannels([m1, m2]);

function build(i, parent) {
    if (i > 0)
        return { parent: parent, array: [1,2,3,4], f: 1.2, label: "hey you", rest1: build(i - 1, this), rest2: build(i - 1, this) }
    else
        return null;
}

var bigdata = build(12, {});

function pingpong(m1:PromiseChannel, m2:PromiseChannel, bigdata)
{
    var t0 = System.getTimer();
    m1.send(bigdata);
    var r = m2.receive();
    var t1 = System.getTimer();
    print('time', t1 - t0);
    if (false) {
        print("false?:", r.rest1.rest1.array === bigdata.rest1.rest1.array);
        print("true?:", r.rest1.rest1.label === bigdata.rest1.rest1.label);
        print("true?:", r.rest1.rest1.array[0] === bigdata.rest1.rest1.array[0]);
        print("true?:", r.rest1.rest1.rest1.parent === r.rest1.rest1.rest2.parent);
    }

}

for (var i:int  = 0; i < 1; i++) {
    pingpong(m1, m2, bigdata);
}
/*
for (var i:int  = 0; i < 1; i++) {
    var data = new ByteArray();
    for (var j:int = 0; j < 1000*1000; j++)
        data.writeByte(j%(2<<8));
    print('data size', data.length);
    t0 = System.getTimer();
    m1.send(data);
    var r = m2.receive();
    t1 = System.getTimer();
    print('time', t1 - t0);

}
*/

m1.send(42);
m2.receive();
Worker.current.stop();
