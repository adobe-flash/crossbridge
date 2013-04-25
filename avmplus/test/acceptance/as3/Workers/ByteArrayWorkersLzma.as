/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package {

    import flash.utils.ByteArray
    import flash.system.Worker
    import flash.system.WorkerDomain
    import avmplus.System
import com.adobe.test.Assert;

    var MSG_NEW=0
    var MSG_READY=1
    var MSG_COMPRESS=2
    var MSG_COMPRESS_ACK=3
    var MSG_UNCOMPRESS=4
    var MSG_UNCOMPRESS_ACK=5
    var MSG_DONE=7;

    var data:ByteArray;
    var message:ByteArray;
    var done:Boolean=false;

    if (Worker.current.isPrimordial) {
//         var SECTION = "Workers";
//         var VERSION = "as3";
//         var TITLE   = "test ByteArray uncompress/compress across workers";

        data=new ByteArray();

    // test 1: compress shareable bytearray
        data.shareable=true;
        data.writeUTF("hello world");
        Assert.expectEq("shareable byte array writeUTF initial value","hello world",data.toString().substring(2));
        Assert.expectEq("shareable byte array writeUTF initial length",13,data.length);
        print("initial: "+data.length+" "+data.toString().substring(2));
        data.compress("lzma");
        Assert.expectEq("shareable byte array compress alters data",true,"hello world"!=data.toString().substring(2));
        Assert.expectEq("shareable byte array compress length",31,data.length);
        print("compress: "+data.length+" "+data.toString().substring(2));
        data.uncompress("lzma");
        Assert.expectEq("shareable byte array uncompress restores initial value","hello world",data.toString().substring(2));
        Assert.expectEq("shareable byte array uncompress restores initial length",13,data.length);

        // test 2: compress/decompress the byte array, test the remote byte array can see the change
        message=new ByteArray();
        message[0]=MSG_NEW;  // status from main
        message[1]=MSG_NEW;  // status from background
        message.shareable=true;

        var worker:Worker=WorkerDomain.current.createWorkerFromPrimordial();
        worker.setSharedProperty("message",message);
        worker.setSharedProperty("data",data);
        worker.start();
        message[0]=MSG_READY;
    
        while (!done) {
            if (message[1]==MSG_READY) {
                print("[0] ready "+data.length+" "+data.toString().substring(2));
                Assert.expectEq("shareable byte array after worker start initial value","hello world",data.toString().substring(2));
                Assert.expectEq("shareable byte array after worker start initial length",13,data.length);
                message[1]=MSG_COMPRESS;
            } else if (message[1]==MSG_COMPRESS_ACK) {
                print("[0] compressed "+data.length+" "+data);
                Assert.expectEq("shareable byte array after worker compress alters data",true,"hello world"!=data.toString().substring(2));
                Assert.expectEq("shareable byte array after worker compress length",31,data.length);
                message[1]=MSG_UNCOMPRESS;
            } else if (message[1]==MSG_UNCOMPRESS_ACK) {
                print("[0] uncompressed "+data.length+" "+data.toString().substring(2));
                Assert.expectEq("shareable byte array after worker uncompress restores initial value","hello world",data.toString().substring(2));
                Assert.expectEq("shareable byte array after worker uncompress restores initial length",13,data.length);
                message[1]=MSG_DONE;
                break;
            }
        }
    } else {
        print("in background worker");
        message=Worker.current.getSharedProperty("message");
        data=Worker.current.getSharedProperty("data");
        message[1]=MSG_READY;

        while (true) {
            if (message[1]==MSG_COMPRESS) {
                data.compress("lzma");
                message[1]=MSG_COMPRESS_ACK;
            } else if (message[1]==MSG_UNCOMPRESS) {
                data.uncompress("lzma");
                message[1]=MSG_UNCOMPRESS_ACK;
            } else if (message[1]==MSG_DONE) {
                break;
            }
        }
        print("[1] done");
        Worker.current.terminate();
    }
}
