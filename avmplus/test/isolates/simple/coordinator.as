/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

import avmplus.System;
import avmplus.File;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.Promise;
import flash.system.WorkerDomain;

function makeCode(name) {
    return File.readByteArray(name);
}

var blob = makeCode("dispatcher.abc");
var w:Worker = WorkerDomain.current.createWorkerFromByteArray(blob);

var promise:Promise = w.start();

var NUM_WORKERS:int = 10;
var NUM_INVOCATIONS:int = 20;

var lock:Promise = Promise.empty();

promise.async.dispatch(NUM_WORKERS, NUM_INVOCATIONS, lock);
lock.receive();
w.stop();

}
