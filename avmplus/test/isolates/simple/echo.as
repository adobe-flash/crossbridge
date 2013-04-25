/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import flash.system.PromiseChannel;
import flash.system.Worker;

var incoming = PromiseChannel.initial[0];
var outgoing = PromiseChannel.initial[1];
function loop() {
    try {
        while(true) {
	    var data = incoming.receive();
	    print('got type', data.constructor);
	    outgoing.send(data);
	    if (data === 42) {
	      // outgoing.send(data);
	        //print('done');
       	        break;
	    }
        }
    } catch (e) {
        //print('echo: oops', e);
    }
}

loop();
try {
    incoming.receive(); // should block
} catch (e) {
    print('problem', e);
}
Worker.current.stop();



