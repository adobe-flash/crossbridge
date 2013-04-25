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
import flash.system.PromiseChannel;
import flash.system.WorkerDomain;

function makeCode(name) {
    return File.readByteArray(name);
}

var blob = makeCode("callee.abc");
var p = WorkerDomain.current.createWorkerFromByteArray(blob);

// start callable returns a promise to an object passed to the worker's
// event loop as a parameter
var promise:Promise = p.start();
print("started callable");

// call remote method printing "Remote Hello" to the screen 
// (no argument, no "real" return value)
var helloPromise:Promise = promise.async::remoteHello();
print("OK"+helloPromise);

print("MAIN: HELLO PROMISE RESOLVED (NULL): "+helloPromise.receive());

// call remote method returning the argument
var echoPromise1:Promise = promise.async::remoteEcho("echo1");
print("MAIN: ECHO PROMISE RESOLVED: "+echoPromise1.receive());

// call remote method adding two Number arguments
var addPromise1:Promise = promise.async::remoteAdd(7, 7);
print("MAIN: ADD (NUMBER) PROMISE RESOLVED: "+addPromise1.receive());

// call remote method adding two String arguments
var addPromise2:Promise = promise.async::remoteAdd("7", "7");
print("MAIN: ADD (STRING) PROMISE RESOLVED: "+addPromise2.receive());

// call remote method taking as an argument a promise returned by
// another remote method, resolving it automatically (as its local to
// the callee), and printing it to the screen (no "real" return value)
var echoPromise2:Promise = promise.async::remoteEcho("echo2");
var printPromise:Promise = promise.async::remotePrint(echoPromise2);
print("MAIN: PRINT PROMISE RESOLVED (NULL): "+printPromise.receive());

var selfPromise = promise.async::remoteSelf();
print("MAIN: Remote self is " + selfPromise);

try {
    var userExceptionPromiseSync:Promise = promise.async::throwRemoteException();
    userExceptionPromiseSync.receive();
 } catch (exception) {
    print("MAIN: REMOTE FUNCTION GENERATED THE USER FOLLOWING EXCEPTION (SYNCHRONOUSLY): "+exception);
 }

try {
    var wrongFunctionNamePromise:Promise = promise.async::blah();
    print(wrongFunctionNamePromise.receive());
 } catch (error) {
    print("MAIN: REMOTE FUNCTION GENERATED THE FOLLOWING SYSTEM ERROR: "+error);
 }


//var remoteInt:int = promise.getTen();
//print("MAIN: Whee, transparent resolution of primitive types:", remoteInt);

//function add2(arg: int):int {
//    return arg + 2;
//}

//print("MAIN: Whee, transparent resolution of primitives as function arguments:",  add2(promise.getTen()));

var staticPrintPromise:Promise = promise.async::CallbackClass.async::remoteStaticPrint("echo3");
staticPrintPromise.receive();

var remoteStrVarPromise:Promise = promise.async::remoteStr;
print("MAIN: accessed remote string variable: "+remoteStrVarPromise.receive());

var strVarPromise:Promise = promise.async::remoteObj.async::str;
print("MAIN: accessed string field of remote object variable: "+strVarPromise.receive());

var emptyPromise:Promise = Promise.empty();
promise.async::passEmpty(emptyPromise);
print("MAIN: empty promise resolved by deep caller: "+emptyPromise.receive());



var callbackPromise = promise.async::callbackObject;
callbackPromise.async::callback("callback from caller");

// wait until last action of deep callee (callback) and last action of
// the main worker (also callback) complete
do {
  var num_calls:int = promise.async::getNumCalls().receive();
  if (num_calls == 4)
    break;
  else 
    System.sleep(0);
 } while (true);

var killPromise:Promise = promise.async::killDeepCallee();
killPromise.receive();

p.stop();

