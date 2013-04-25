/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.Promise;
import flash.system.WorkerDomain;

// Run as, e.g., avm fanout-with-exit.abc  -- 5  to get 5 levels of process branching


if (Worker.current.isPrimordial()) {
    spawnMany(int(System.argv[0]), null, null);
    Promise.eventLoop();
}
class ChildCounter {
    var counter:int = 0;
    public function inc() { counter ++; print(Worker.current, "childCount", counter); }
    public function dec() { 
        counter --; 
        print(Worker.current, "childCount", counter); 
        if (counter == 0)  {
            print(Worker.current, "done");
            Worker.current.stop();
        }
    }
}

function spawnMany(count:int, parentRef, parent) 
{
    print(Worker.current, 'spawning', count, 'children');
    try {
        var arr = [];
        var counter = new ChildCounter();
        for (var i = 0; i < count; i++) {
            var isolate = WorkerDomain.current.createWorkerFromPrimordial();
            var p:Promise = isolate.start();
            if (p) {
                arr.push(isolate);
                counter.inc();
                p.async::spawnMany(count - 1, Promise.wrap(counter), Worker.current);
            }
        }
        if (arr.length == 0) {
            if (count != 0)
                print(Worker.current, "failed to spawn children");
            else
                print(Worker.current, "childless");
        }
    } catch (e) {
        print("whoops", e);
    } finally {
        if (parentRef) {
            parentRef.async::dec();
            System.sleep(1500); // this is to give the parent enough time to get and process messages
            // FIXME: instead: gentle exit, when all outgoing messages are consumed: Worker::scheduleExit()
            Worker.current.stop();

        }
    }
}



