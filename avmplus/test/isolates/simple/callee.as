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


function remoteHello() {
    print("Remote Hello");
}

function remoteEcho(s:String) {
    return s;
}

function remoteAdd(arg1, arg2) {
    return arg1+arg2;
}

function remotePrint(s:String) {
    print("CALEE: "+s);
}

function remoteSelf() {
    return this;
}


function throwRemoteException() {
    throw "exception on callee thrown";
}

function getTen():int {
    return 10;
}

function makeCode(name) {
    return File.readByteArray(name);
}

var blob = makeCode("deep_callee.abc");
var p = WorkerDomain.current.createWorkerFromByteArray(blob);

function killDeepCallee():void
{
  p.stop();
}

var promise:Promise = p.start();
print("started deep callable");

function passEmpty(emptyPromise:Promise) {
  promise.async::resolveEmpty(emptyPromise);
}

var num_calls = 0;
class CallbackClass
{
  public function callback(arg:String):void
  {
    remotePrint(arg);
    num_calls++;
  }

  public static function remoteStaticPrint(s:String):void
  {
    print("CALEE: "+s);
  }

};

function getNumCalls():int
{
  return num_calls;
}

var remoteStr:String = "echo4";

var remoteObj = {str: remoteStr};


var callbackObject = new CallbackClass();
  
function getCallbackObject():Object
{
  return callbackObject;
}

var asynchPromise = promise.async::remoteEcho("asynchronously called back");

function successHandler(p:Promise)
{
  // p.resolve() is guaranteed to be non-blocking here
  print(p.receive());
  num_calls++;

}

asynchPromise.when(successHandler);

var userExceptionPromiseNonSync:Promise = promise.async::throwRemoteException();

function faultHandler(p:Promise)
{
    try {
	p.receive();
    } catch (exception) {
	print("MAIN: REMOTE FUNCTION GENERATED THE USER FOLLOWING EXCEPTION (NON-SYNCHRONOUSLY): "+exception);
    }
    num_calls++;
}

userExceptionPromiseNonSync.when(null, faultHandler);


//request callback from deep callee
var localPromise = Promise.wrap(callbackObject);
promise.async::executeCallback(localPromise);

