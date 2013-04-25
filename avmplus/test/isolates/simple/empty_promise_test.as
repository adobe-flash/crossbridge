/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

import avmplus.System;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.Promise;
import flash.system.WorkerDomain;

if (Worker.current.isPrimordial) {
  
    var p = WorkerDomain.current.createWorkerFromPrimordial();
    
    var promise:Promise = p.start();
    
    var empty_sync:Promise = Promise.empty();    
    var empty_async:Promise = Promise.empty();
    promise.async.foo(empty_async, "UNSYNCH");

    function handler(/*e_a:Promise*/receivedResult) {
	print(receivedResult/*e_a.receive()*/);

	promise.async.foo(empty_sync, "SYNCH");
    }

    empty_async.when(handler);

    var res = empty_sync.receive();
    print(res);
    try {
      empty_sync.resolve("DUMMY");
    }
    catch (ex) {
      print("SECOND RESOLUTION ATTEMPT: "+ex);
    }
    p.stop();


 }
 else {
     public function foo(e:Promise, s:String) {
	 e.resolve(s);
     }

 }
}

