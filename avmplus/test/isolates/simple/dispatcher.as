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
import flash.system.PromiseChannel;
import flash.system.WorkerDomain;

function makeCode(name) {
    return File.readByteArray(name);
}

// total number of invocations (initialized in the dispatch method)
var NUM_INVOCATIONS:int;

// number of workers to be scheduled
var NUM_WORKERS:int;

// count how many invocations have been started
var startedInvocations:int = 0;
// count how many invocations have been completed
var doneInvocations:int = 0;

var workers = new Array();
// array representing global scopes of service workers
// used to schedule computations on these workers
var promises:Array = new Array();

var lock:Promise;

// count how many computations are completed and, if not all of them
// are done, schedule additional ones
function serviceHandler(/*p:Promise*/receivedResult) {
    var promise_slot:int = /*p.receive()*/ receivedResult as int;
    doneInvocations++;   
    if (startedInvocations == NUM_INVOCATIONS) {
	// don't exceed the number of computations to perform
	workers[promise_slot].stop();
    }
    else {
        var res:Promise = promises[promise_slot].async.printRequestID(NUM_WORKERS+doneInvocations, promise_slot);
	startedInvocations++;
	res.when(arguments.callee);
    }    
    if (doneInvocations == NUM_INVOCATIONS)
	// last computation done - notify coordinator
	lock.resolve("DUMMY");
}

public function dispatch(numWorkers:int, numInvocations:int, l:Promise) {
    
    lock = l;

    NUM_WORKERS = numWorkers;
    NUM_INVOCATIONS = numInvocations;
    var blob = makeCode("service.abc");

    for(var i:int = 0; i<NUM_WORKERS; i++) {
	workers[i] = WorkerDomain.current.createWorkerFromByteArray(blob);
	promises[i] = workers[i].start();
	// schedule the first 10 computations; the first argument is
	// what's printed by the service worker; the second argument
	// represents a slot in promises array so that we can schedule
	// subsequent computation in the handler	
	var res:Promise = promises[i].async.printRequestID(i, i);
	startedInvocations++;
	res.when(serviceHandler);
    }
}

}    
