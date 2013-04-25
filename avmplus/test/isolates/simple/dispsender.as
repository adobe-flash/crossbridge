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

var blob = makeCode("dispreceiver.abc");
var p = new Worker([blob]);
var m1 = new PromiseChannel(Worker.currentWorker, p);
var m2 = new PromiseChannel(Worker.currentWorker, p);
var m3 = new PromiseChannel(p, Worker.currentWorker);

p.startWithChannels([m1, m2, m3]);

class Foo {
    var x:int = 10;
};

m1.send(new Foo());
Worker.registerClassAlias("Foo", Foo);
m1.send(new Foo());

m1.send(<foo><bar></bar></foo>);
m1.send(new Date());
m1.send(p);
var ch = new PromiseChannel(Worker.currentWorker, p);
m1.send(ch);

//ch.send(10);
m1.send(blob);

m1.send(10);
m2.send(true);
m1.send(undefined);


m1.send({x:1});

function build(i) {
    if (i > 0)
        return { array: [1,2,3,4], f: 1.2, label: "hey", rest: build(i - 1) }
    else
        return null;
}

var t0,t1;
print('iterating sending big data');
for (var i = 0; i < 5; i++) {
    var bigdata = build(1000);
    t0 = System.getTimer();
    m1.send(bigdata);
    //t1 = m3.receive();
}

print('time', t1 - t0);
Worker.currentWorker.exit();