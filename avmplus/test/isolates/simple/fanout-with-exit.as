/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */i

import avmplus.System;
import avmplus.File;
import flash.utils.ByteArray;
import flash.system.Worker;

// Run as, e.g., fanout-with-exit.abc a b c d e, the command line arg values don't matter, only their number.

function makeCode(name) {
    return File.readByteArray(name);
}

if (System.argv.length > 0) {
    var newlen = System.argv.length - 1;
    var newarg = System.argv.slice(0, newlen);
    
    print(Worker.currentWorker, 'with args', System.argv, 'spawning', newlen, 'children');
    var arr = [];
    for (var i = 0; i < newlen; i++) {
       var isolate = new Worker([makeCode("fanout-with-exit.abc")], newarg);
       if (isolate.startWithChannels())
            arr.push(isolate);
   }

   for (var i = 0; i < arr.length; i++) {
       System.sleep(50*Math.random());
       arr[i].exit();
       print(arr[i], 'exit');
   }
   if (arr.length == 0)
      print(Worker.currentWorker, "failed to spawn children");
       
} else {
    print('done');
}
