/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.system.Worker;
import flash.system.PromiseChannel;
class ReceiverHandler {

    public function ReceiverHandler(inc: PromiseChannel, out:PromiseChannel) {
        this.inc = inc;
        this.out = out;
    }

    var out:PromiseChannel;
    var inc:PromiseChannel;
    public var client:Object;

    function handle():Boolean {
        var msg = inc.receive();
        if (msg === 42) return false;
        var name = msg.name;
        var args = msg.args;
        var result = client[name].apply(client, args);
        out.send(result);
        return true;
    }
}

var h:ReceiverHandler = new ReceiverHandler(PromiseChannel.initial[0], PromiseChannel.initial[1]);
class Client {
    function hello(s) {
        print('hello', s);
    }
    function bye(s) {
        print('bye', s);
    }
}

h.client = new Client();

while(h.handle())
   ;





