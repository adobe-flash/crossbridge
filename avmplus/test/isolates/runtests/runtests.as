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

var start = getTimer();

class MyPromise extends Promise {
    public var id:int;
}

function getTests():Array {
    if (!File.exists('tests.txt')) {
        print('Test runner requires a line delimited tests.txt file.');
        System.exit(1);
    }
    
    var tests = File.read('tests.txt');
    return tests.split('\n');
}

function loadCode(file):ByteArray {
    return File.readByteArray(file);
}

function startWorker(test):Object {
    //print('Starting test: '+test);
    var w:Worker = new Worker([loadCode(test)]);
    var p:Promise = w.start();
    //print('Adding worker: '+w);
    return {'w':w,'name':test};
}

var tests:Array = getTests();

var maxWorkers:int = 4;
var workerCount:int = 0;
var workers:Array = [];

var c =0;

while (true) {
    if (tests.length == 0) {
        break;
    } else if (workers.length < maxWorkers) {
        // start a new worker - store into the array
        var workerObj = startWorker(tests.pop());
        print('WorkerObj: '+workerObj['name']+' '+workerObj['w'])
        workers.push(workerObj)
    } else {
        var elementsToRemove = [];
        for (var i=0; i<workers.length; i++) {
            var state = workers[i]['w'].state();
            if (state == Worker.EXITED) {
                print('EXITED: '+workers[i]['name']);
                elementsToRemove.push(i);
            } else if (state > Worker.EXITED) {
                print('ERROR: '+ state + ' | ' +workers[i]['name']);
                elementsToRemove.push(i);
            }
        }
        print('elementsToRemove: '+elementsToRemove);
        // Destroy workers and remove from array
        for (var i=0; i<elementsToRemove.length; i++) {
            print('Destroying Worker # '+elementsToRemove[i]);
            // TODO: why sometimes the references to finished workers
            // are undefined???
            if (workers[elementsToRemove[i]] != undefined) {
                var state = workers[elementsToRemove[i]]['w'].exit(0);
                print('Clean exit for '+workers[elementsToRemove[i]]['name']+' = '+state);
            }
            // remove from array
            workers.splice(elementsToRemove[i],1);
        }
    }
    System.sleep(50);
}


print('done');

var end = getTimer() - start;
File.write('time.txt', end);

Worker.currentWorker.exit(0);
System.exit(0);

