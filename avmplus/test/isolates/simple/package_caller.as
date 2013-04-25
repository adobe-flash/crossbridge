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

var blob = makeCode("package_callee.abc");
var p = WorkerDomain.current.createWorkerFromByteArray(blob);
var promise:Promise = p.start();
print("MAIN: call to baz.foo() returned "+promise.baz.foo().local::receive());
print("MAIN: call to baz.bar.foo() returned "+promise.baz.bar.foo().local::receive());
print("MAIN: call to foo() returned "+promise.foo().local::receive());
print("MAIN: baz.V1's value is "+promise.baz.v1.local::receive());
print("MAIN: baz.bar.V2's value is "+promise.baz.bar.v2.local::receive());
print("MAIN: V3's value is "+promise.v3.local::receive());
print("MAIN: call to unnamedFoo() in unnamed package returned "+promise.unnamedFoo().local::receive());

try {
  promise.v4.local::receive();
 }
catch (ex) {
  print("MAIN ("+ex+"): Tried accessing non-existent variable v4 in global scope");
}

try {
  promise.baz.v4.local::receive();
 }
catch (ex) {
  print("MAIN ("+ex+"): Tried accessing non-existent variable baz.v4");
}


try {
  promise.baz.bar.v4.local::receive();
 }
catch (ex) {
  print("MAIN ("+ex+"): Tried accessing non-existent variable baz.bar.v4");
}

print("MAIN: Tried accessing existing variable bam (7777) in global scope - takes precedence over package prefix: "+promise.bam.local::receive());

print("MAIN: Tried accessing existing variable bar (77777) in global scope - package bar takes precedence: "+promise.bar.local::receive());

print("MAIN: Tried accessing existing class variable redef (0) in global scope - variable redef from package baz (42) takes precedence): "+promise.baz.redef.local::receive());

p.stop();
