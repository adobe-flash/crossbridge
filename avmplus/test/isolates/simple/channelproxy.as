/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.utils.Proxy;
import flash.utils.flash_proxy;
import avmplus.System;
import avmplus.File;
import flash.system.Worker;
import flash.system.PromiseChannel;


class SenderProxy extends Proxy {

    public function SenderProxy(receiver:Worker) {
        out = new PromiseChannel(Worker.current, receiver);
        inc = new PromiseChannel(receiver, Worker.current);
    }

    public var out:PromiseChannel;
    public var inc:PromiseChannel;

    flash_proxy override function callProperty(name:*, ...rest):* {
        try {
            // investigate why toString() is needed.
            out.send({name: name.toString(), args:rest});
            return inc.receive();
        } catch (e:Error) {
            // respond to error here
        }
    }
    static function close(p: SenderProxy) {
        p.out.send(42);
    }
    
}



function makeCode(name) {
    return File.readByteArray(name);
}

var p = new Worker([makeCode("echoproxy.abc")]);
var proxy = new SenderProxy(p);

p.startWithChannels([proxy.out, proxy.inc]);

proxy.hello('world');

proxy.bye('kitty');

SenderProxy.close(proxy);
