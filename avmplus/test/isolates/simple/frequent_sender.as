/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.System;
import avmplus.File;
import flash.utils.ByteArray;
import flash.system.Worker;
import flash.system.PromiseChannel;

const NUM_MSGS:Number = 100000;

function makeCode(name) {
    return File.readByteArray(name);
}

if (System.argv.length > 0) {
  var senders_num:Number = Number(System.argv[0]);
  var senders:Array = new Array(senders_num);
  for (var i=0; i<senders_num; i++) {
    senders[i] = new Worker([makeCode("frequent_sender.abc")]);
  }
  for (var i=0; i<senders_num; i++) {
    senders[i].startWithChannels();
  }
 }
 else {
   var p = new Worker([makeCode("frequent_receiver.abc")]);
   var m1 = new PromiseChannel(Worker.current, p);
   var m2 = new PromiseChannel(p, Worker.current);
   var a:Array = new Array(NUM_MSGS);
   
   for (var i = 0; i < NUM_MSGS; i++) {
     //  a[i] = { array: [1,2,3,4], n: i, label: "hey" };
     a[i] = i;
   }

   p.startWithChannels([m1, m2]);
   m1.send(NUM_MSGS);

   print('sending data');
   var t0 = System.getTimer();
   for (var i = 0; i < NUM_MSGS; i++) {
     m1.send(a[i]);
   }
   var t1 = System.getTimer();

   // a barrier (in an attempt to get more accurate time measurements) -
   // wait before entering the timed loop until all receivers receive all
   // their data
   m1.send("SYNC");
   var msg = m2.receive();

   print('receiving data');
   var t2 = System.getTimer();
   for (var i = 0; i < NUM_MSGS; i++) {
     a[i] = m2.receive();
   }
   var t3 = System.getTimer();
   
   
   print('send time', t1 - t0);
   print('receive time', t3 - t2);
   p.stop();
   Worker.current.stop();
 }
