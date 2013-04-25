/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.system.Worker;
import flash.system.PromiseChannel;
import avmplus.File;
import flash.utils.ByteArray;

import avmplus.System;

if (Worker.current.isPrimordial()) 
    senderCode();
else
    receiverCode();


class Foo {
    var x:int = 10;
}
    
dynamic class Bar extends Foo {
    var y:int = 12;
}
    
class Baz extends Bar {
    var w:int = 100;
    print("don't run me in every worker!", Worker.current);
}

/*
dynamic class FunnyArray extends Array {
    var funnyLength:int = 42;
};
*/
    
function senderCode() 
{

    
    var p = new Worker();
    var m1 = new PromiseChannel(Worker.current, p);
    var m2 = new PromiseChannel(Worker.current, p);
    var m3 = new PromiseChannel(p, Worker.current);
    
    p.startWithChannels([m1, m2, m3]);
    
    m1.send(new Foo());
    var bar1 = new Bar();
    bar1.z = 15;
    m1.send(bar1);
    var baz1 = new Baz();
    //(baz1 as Bar).z = -1;
    m1.send(baz1);

    /*
    var funnyArray1 = new FunnyArray();
    funnyArray1[4] = 10;
    m1.send(funnyArray1);
    */

    m1.send(<foo><bar></bar></foo>);
    m1.send(new Date());
    m1.send(p);
    var ch = new PromiseChannel(Worker.current, p);
    m1.send(ch);
    
    ch.send(10);
    m1.send(File.readByteArray("type_exchange.abc"));
    
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
        t1 = m3.receive();
    }
    
    print('time', t1 - t0);
    Worker.current.stop();
}

function receiverCode() 
{
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
    
    
}

