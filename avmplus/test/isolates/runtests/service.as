/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*
import flash.system.Worker;
import flash.system.Promise;
import flash.system.WorkerDomain;

var prefix="../../"

var counter:int=0;
function runtest(test:String, promise_slot:int):int {
  var start:int=getTimer();  
  print("*** starting worker: "+promise_slot+" count: "+counter+" test: "+test);
  var fulltest=prefix+test;
  counter++;
  try {
      Domain.currentDomain.load(fulltest);
  } catch (e) {
      print("uncaught exception: "+e);
  }
  print("*** finished worker: "+promise_slot+" count: "+counter+" time: "+(getTimer()-start)+" test: "+test);
  return promise_slot;
}
