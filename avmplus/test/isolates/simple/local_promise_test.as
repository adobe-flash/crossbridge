/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import flash.system.*

var done:Boolean = false;

var bar = "BAR";    
var globalPromise:Promise = Promise.wrap(this);
var o = {f:foo, b:bar};
var promise = Promise.wrap(o);

function foo() {
    return "FOO";
}

if (Worker.current.isPrimordial()) {    
    print("FOO from global promise call? "+globalPromise.foo().local::receive());
    print("BAR from global promise property? "+globalPromise.bar.local::receive());
    print("FOO from regular promise call? "+promise.f().local::receive());
    print("BAR from regular promise propety? "+promise.b.local::receive());

    var w:Worker=WorkerDomain.current.createWorkerFromPrimordial();
    var p:Promise=w.start();

    do {
	var finish:Boolean = p.done.local::receive();
	if (finish) break;
	else System.sleep(0);
    } while (true);
    
    w.stop();

 } else {

    function handler(p:Promise) {
	print("FOO from regular asynchronous promise call? "+p.local::receive());
	done = true;
    }

    var r:Promise = promise.f()
    r.local::when(handler);
 }