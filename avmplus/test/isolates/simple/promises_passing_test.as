/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import avmplus.File;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.Promise;
import flash.system.WorkerDomain;


function makeCode(name) {
    return File.readByteArray(name);
}

var blob = makeCode("first_promise_tester.abc");
var p = WorkerDomain.current.createWorkerFromByteArray(blob);
var promise:Promise = p.start();


var startTestResult:Promise = promise.startTest();
startTestResult.local::receive(); // wait for completion

p.stop();
